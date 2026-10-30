#include "preview.h"

#include <algorithm>
#include <cstdio>
#include <string>

#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

namespace {
const char *window_name = nullptr;
}

extern "C" int preview_init(const char *name) {
  window_name = name;
  try {
    cv::namedWindow(window_name, cv::WINDOW_AUTOSIZE);
  } catch (const cv::Exception &error) {
    std::fprintf(stderr, "Could not create preview window: %s\n", error.what());
    window_name = nullptr;
    return 0;
  }
  return 1;
}

extern "C" int preview_frame(const uvc_frame_t *frame) {
  cv::Mat image;
  uvc_frame_t *bgr = nullptr;
  const uint8_t *metadata_bytes;
  char line[256];
  int key;

  if (!window_name || !frame) return 0;
  try {
    if (frame->frame_format == UVC_FRAME_FORMAT_MJPEG) {
      cv::Mat compressed(1, (int)frame->data_bytes, CV_8UC1, frame->data);
      image = cv::imdecode(compressed, cv::IMREAD_COLOR);
      if (image.empty()) {
        std::fprintf(stderr, "Could not decode MJPEG frame for preview\n");
        return 0;
      }
    } else {
      bgr = uvc_allocate_frame(frame->width * frame->height * 3);
      if (!bgr) {
        std::fprintf(stderr, "Could not allocate preview frame\n");
        return 0;
      }
      if (uvc_any2bgr(const_cast<uvc_frame_t *>(frame), bgr) != UVC_SUCCESS) {
        std::fprintf(stderr, "Could not convert frame format %d for preview\n",
            (int)frame->frame_format);
        uvc_free_frame(bgr);
        return 0;
      }
      image = cv::Mat(bgr->height, bgr->width, CV_8UC3, bgr->data, bgr->step);
    }
  } catch (const cv::Exception &error) {
    std::fprintf(stderr, "Could not prepare frame for preview: %s\n", error.what());
    if (bgr) uvc_free_frame(bgr);
    return 0;
  }

  std::snprintf(line, sizeof(line), "frame %u  %u.%03u  %ux%u  format %d",
      frame->sequence, (unsigned)frame->capture_time_finished.tv_sec,
      (unsigned)(frame->capture_time_finished.tv_nsec / 1000000),
      frame->width, frame->height, (int)frame->frame_format);
  cv::putText(image, line, cv::Point(12, 24), cv::FONT_HERSHEY_SIMPLEX,
      0.55, cv::Scalar(0, 0, 0), 3, cv::LINE_AA);
  cv::putText(image, line, cv::Point(12, 24), cv::FONT_HERSHEY_SIMPLEX,
      0.55, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
  std::snprintf(line, sizeof(line), "payload %zu bytes  metadata %zu bytes",
      frame->data_bytes, frame->metadata_bytes);
  cv::putText(image, line, cv::Point(12, 48), cv::FONT_HERSHEY_SIMPLEX,
      0.55, cv::Scalar(0, 0, 0), 3, cv::LINE_AA);
  cv::putText(image, line, cv::Point(12, 48), cv::FONT_HERSHEY_SIMPLEX,
      0.55, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
  if (frame->metadata_bytes) {
    std::string metadata = "metadata: ";
    const size_t displayed_bytes = std::min(frame->metadata_bytes, (size_t)24);
    metadata_bytes = static_cast<const uint8_t *>(frame->metadata);
    for (size_t index = 0; index < displayed_bytes; ++index) {
      char byte_text[4];
      std::snprintf(byte_text, sizeof(byte_text), "%02x", metadata_bytes[index]);
      metadata += byte_text;
      if (index + 1 < displayed_bytes) metadata += ' ';
    }
    if (displayed_bytes < frame->metadata_bytes) metadata += " ...";
    cv::putText(image, metadata, cv::Point(12, 72), cv::FONT_HERSHEY_SIMPLEX,
        0.45, cv::Scalar(0, 0, 0), 3, cv::LINE_AA);
    cv::putText(image, metadata, cv::Point(12, 72), cv::FONT_HERSHEY_SIMPLEX,
        0.45, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
  }

  try {
    cv::imshow(window_name, image);
    key = cv::waitKey(1) & 0xff;
  } catch (const cv::Exception &error) {
    std::fprintf(stderr, "Preview display failed: %s\n", error.what());
    if (bgr) uvc_free_frame(bgr);
    return 0;
  }
  if (bgr) uvc_free_frame(bgr);
  return key == 'q' || key == 27 ? 1 : 0;
}

extern "C" void preview_close(void) {
  if (window_name) {
    cv::destroyWindow(window_name);
    window_name = nullptr;
  }
}