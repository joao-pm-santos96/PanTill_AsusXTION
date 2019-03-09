/**
 * @file openni2.cpp
 * @author João Santos, 76912, MIEM-UA (santos.martins.joao@ua.pt)
 * @brief this file contains the functions needes to reatrive color and depth image from the Asus Xtion
 * @version 0.1
 * @date 2019-01-18
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "pari_bbl.h"

openni::Device device; /** stores the info about the device opened */
openni::VideoStream depth_stream; /** stores the info about the depth frame */
openni::VideoStream color_stream; /** stores the info about the color frame */

/**
 * @brief function needed to start the OpenNI camera, in this case, the Asus Xtion.
 * 
 * Is iniatializes the device, sets the pretended definitions, opens it and starts the depth and color streams (based on: https://github.com/gaug-cns/openni2-recorder).
 * 
 * @return gboolean 
 */
gboolean InitOpenNI()
{
    using namespace openni;

    // Initialize OpenNI
    if (OpenNI::initialize() != STATUS_OK)
    {
        std::cerr << "OpenNI Initial Error: " << OpenNI::getExtendedError() << std::endl;
        return FALSE;
    }

    // Open Device
    //Device device;
    if (device.open(ANY_DEVICE) != STATUS_OK)
    {
        std::cerr << "Can't Open Device: " << OpenNI::getExtendedError() << std::endl;
        return FALSE;
    }

    // Create depth stream
    //VideoStream depth_stream;
    if (depth_stream.create(device, SENSOR_DEPTH) == STATUS_OK)
    {
        // 3a. set video mode
        VideoMode mode;
        mode.setResolution(size.width, size.height);
        mode.setFps(30);
        mode.setPixelFormat(PIXEL_FORMAT_DEPTH_1_MM);

        if (STATUS_OK != depth_stream.setVideoMode(mode))
        {
            std::cout << "Can't apply VideoMode: " << OpenNI::getExtendedError() << std::endl;
        }
    }
    else
    {
        std::cerr << "Can't create depth stream on device: " << OpenNI::getExtendedError() << std::endl;
        return FALSE;
    }

    // Create color stream
    //VideoStream color_stream;
    if (device.hasSensor(SENSOR_COLOR))
    {
        if (STATUS_OK == color_stream.create(device, SENSOR_COLOR))
        {
            // 4a. set video mode
            VideoMode mode;
            mode.setResolution(size.width, size.height);
            mode.setFps(30);
            mode.setPixelFormat(PIXEL_FORMAT_RGB888);

            if (STATUS_OK != color_stream.setVideoMode(mode))
            {
                std::cout << "Can't apply VideoMode: " << OpenNI::getExtendedError() << std::endl;
            }

            // 4b. image registration
            if (device.isImageRegistrationModeSupported(IMAGE_REGISTRATION_DEPTH_TO_COLOR))
            {
                device.setImageRegistrationMode(IMAGE_REGISTRATION_DEPTH_TO_COLOR);
            }
        }
        else
        {
            std::cerr << "Can't create color stream on device: " << OpenNI::getExtendedError() << std::endl;
            return FALSE;
        }
    }

    depth_stream.start();
    color_stream.start();

    return TRUE;
}

/**
 * @brief Function that captures and stores the color frame from the Asus Xtion, using OpenNI (based on: https://github.com/gaug-cns/openni2-recorder).
 * 
 * In the case where the frame cannot be retrieved, a 10 by 10 image is sent so it can be taken a decision in tha calling function.
 * 
 * @return cv::Mat 
 */
cv::Mat UpdateNIFrame_Color()
{
    using namespace openni;

    VideoFrameRef color_frame;

    cv::Mat Nula(10, 10, CV_8UC3, cv::Scalar(255, 255, 255)); 

    // Read first frame
    if (color_stream.readFrame(&color_frame) != STATUS_OK)
    {
        std::cerr << "Can't read color frame." << std::endl;
        return Nula;
    }

    // Check possible errors
    if (!color_stream.isValid())
    {
        std::cerr << "Color stream not valid" << std::endl;
        return Nula;
    }

    if (STATUS_OK != color_stream.readFrame(&color_frame))
    {
        std::cerr << "Can't read color frame'" << std::endl;
        return Nula;
    }

    // Grab frames, result is frame and frame_depth
    const cv::Mat color_frame_temp(size, CV_8UC3, (void *)color_frame.getData());
    cv::Mat frame_color;
    cv::cvtColor(color_frame_temp, frame_color, CV_RGB2BGR);

    //cv::flip(frame_color, frame_color, 1);

    return frame_color;
}

/**
 * @brief Function that captures and stores the depth frame from the Asus Xtion, using OpenNI (based on: https://github.com/gaug-cns/openni2-recorder).
 * 
 * It also applies the desires ColorMap (https://docs.opencv.org/3.1.0/d3/d50/group__imgproc__colormap.html).
 *
 * In the case where the frame cannot be retrieved, a 10 by 10 image is sent so it can be taken a decision in tha calling function.
 * 
 * @return cv::Mat 
 */
cv::Mat UpdateNIFrame_Depth(int ColorMap)
{
    using namespace openni;

    VideoFrameRef depth_frame;
    int max_depth = depth_stream.getMaxPixelValue();

    cv::Mat Nula(10, 10, CV_8UC3, cv::Scalar(255, 255, 255)); 

    // Read first frame
    if (depth_stream.readFrame(&depth_frame) != STATUS_OK)
    {
        std::cerr << "Can't read depth frame." << std::endl;
        return Nula;
    }

    // Check possible errors
    if (STATUS_OK != depth_stream.readFrame(&depth_frame))
    {
        std::cerr << "Can't read depth frame'" << std::endl;
        return Nula;
    }

    // Grab frames, result is frame and frame_depth
    const cv::Mat depth_frame_temp(size, CV_16UC1, (void *)depth_frame.getData());
    cv::Mat frame_depth;
    depth_frame_temp.convertTo(frame_depth, CV_8U, 255. / max_depth);
    cv::Mat frame_depth_color;

    //cv::cvtColor(frame_depth, frame_depth_color, CV_GRAY2BGR);
    cv::applyColorMap(frame_depth, frame_depth_color, ColorMap);

    return frame_depth_color;
}

/**
 * @brief Function that closes all OpenNI devices and streams.
 * 
 * It destroys both streams, closes the device and shuts down the OpenNI session (based on: https://github.com/gaug-cns/openni2-recorder).
 * 
 * @return gboolean
 */
gboolean CloseOpenNI()
{
    using namespace openni;

    // Destroy objects, otherwise errors will occure next time...
    depth_stream.destroy();
    color_stream.destroy();
    device.close();
    OpenNI::shutdown();

    return TRUE;
}
