#ifndef TOF_EXAMPLES_DATA_STRUCTURES
#define TOF_EXAMPLES_DATA_STRUCTURES

#include <chronoptics/tof/data.hpp>
#include <cstdint>

struct XYZ {
  float x;
  float y;
  float z;
  float padding;
};

struct XYZAmp {
  float x;
  float y;
  float z;
  float amplitude;
};

struct BGR {
  uint8_t b;
  uint8_t g;
  uint8_t r;
};

static_assert(sizeof(BGR) == 3, "Expected struct size");

struct XYZBGR {
  float x;
  float y;
  float z;
  uint8_t b;
  uint8_t g;
  uint8_t r;
  uint8_t padding;
};

struct XYZBGRI {
  float x;
  float y;
  float z;
  uint8_t b;
  uint8_t g;
  uint8_t r;
  uint8_t intensity;
};

template <typename T>
T* cast_data_safe(chronoptics::tof::Data&) = delete;

template <>
float* cast_data_safe<float>(chronoptics::tof::Data& data) {
  using FT = chronoptics::tof::FrameType;

  auto frame_type = data.frame_type();
  if (frame_type != FT::REAL && frame_type != FT::IMAGINARY &&
      frame_type != FT::PHASE && frame_type != FT::AMPLITUDE &&
      frame_type != FT::X && frame_type != FT::Y && frame_type != FT::Z) {
    throw std::runtime_error("Not using correct frame type");
  }

  return reinterpret_cast<float*>(data.data());
}

template <>
uint8_t* cast_data_safe<uint8_t>(chronoptics::tof::Data& data) {
  using FT = chronoptics::tof::FrameType;

  auto frame_type = data.frame_type();
  if (frame_type != FT::YUV && frame_type != FT::MJPEG &&
      frame_type != FT::INTENSITY) {
    throw std::runtime_error("Not using correct frame type");
  }

  return reinterpret_cast<uint8_t*>(data.data());
}

template <>
uint16_t* cast_data_safe<uint16_t>(chronoptics::tof::Data& data) {
  using FT = chronoptics::tof::FrameType;

  auto frame_type = data.frame_type();
  if (frame_type != FT::RAW_COMMON && frame_type != FT::COMMON &&
      frame_type != FT::RADIAL && frame_type != FT::RAW_AB) {
    throw std::runtime_error("Not using correct frame type");
  }

  return reinterpret_cast<uint16_t*>(data.data());
}

template <>
int16_t* cast_data_safe<int16_t>(chronoptics::tof::Data& data) {
  using FT = chronoptics::tof::FrameType;

  auto frame_type = data.frame_type();
  if (frame_type != FT::RAW) {
    throw std::runtime_error("Not using correct frame type");
  }

  return reinterpret_cast<int16_t*>(data.data());
}

template <>
BGR* cast_data_safe<BGR>(chronoptics::tof::Data& data) {
  using FT = chronoptics::tof::FrameType;

  auto frame_type = data.frame_type();
  if (frame_type != FT::BGR && frame_type != FT::BGR_PROJECTED) {
    throw std::runtime_error("Not using correct frame type");
  }

  return reinterpret_cast<BGR*>(data.data());
}

template <>
XYZ* cast_data_safe<XYZ>(chronoptics::tof::Data& data) {
  using FT = chronoptics::tof::FrameType;

  auto frame_type = data.frame_type();
  if (frame_type != FT::XYZ) {
    throw std::runtime_error("Not using correct frame type");
  }

  return reinterpret_cast<XYZ*>(data.data());
}

template <>
XYZAmp* cast_data_safe<XYZAmp>(chronoptics::tof::Data& data) {
  using FT = chronoptics::tof::FrameType;

  auto frame_type = data.frame_type();
  if (frame_type != FT::XYZ_AMP && frame_type != FT::BGR_PROJECTED) {
    throw std::runtime_error("Not using correct frame type");
  }

  return reinterpret_cast<XYZAmp*>(data.data());
}

template <>
XYZBGR* cast_data_safe<XYZBGR>(chronoptics::tof::Data& data) {
  using FT = chronoptics::tof::FrameType;

  auto frame_type = data.frame_type();
  if (frame_type != FT::XYZ_BGR) {
    throw std::runtime_error("Not using correct frame type");
  }

  return reinterpret_cast<XYZBGR*>(data.data());
}

template <>
XYZBGRI* cast_data_safe<XYZBGRI>(chronoptics::tof::Data& data) {
  using FT = chronoptics::tof::FrameType;

  auto frame_type = data.frame_type();
  if (frame_type != FT::XYZ_BGR_I) {
    throw std::runtime_error("Not using correct frame type");
  }

  return reinterpret_cast<XYZBGRI*>(data.data());
}

#endif