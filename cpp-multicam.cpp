// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2015 Intel Corporation. All Rights Reserved.

#include <librealsense/rs.hpp>
#include "example.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>


#include <iostream>
#include <algorithm>

std::vector<texture_buffer> buffers;

int main(int argc, char * argv[]) try
{
	rs::log_to_console(rs::log_severity::warn);
	//rs::log_to_file(rs::log_severity::debug, "librealsense.log");

	rs::context ctx;
	if (ctx.get_device_count() == 0) throw std::runtime_error("No device detected. Is it plugged in?");

	// Enumerate all devices
	std::vector<rs::device *> devices;
	for (int i = 0; i<ctx.get_device_count(); ++i)
	{
		devices.push_back(ctx.get_device(i));
	}

	auto colorStream = rs::stream::color;
	auto depthStream = rs::stream::depth;
	auto irstream = rs::stream::infrared;

	// Configure and start our devices
	for (auto dev : devices)
	{
		std::cout << "Starting " << dev->get_name() << "... ";
		dev->enable_stream(depthStream, rs::preset::best_quality);
		dev->enable_stream(colorStream, rs::preset::best_quality);
		dev->start();
		std::cout << "done." << std::endl;
	}

	// Depth and color
	buffers.resize(ctx.get_device_count() * 2);

	char * d1_color_window = "Device 1 Color Stream";
	char * d1_depth_window = "Device 1 Depth Stream";
	char * d2_color_window = "Device 2 Color Stream";
	char * d2_depth_window = "Device 2 Depth Stream";
	cv::namedWindow(d1_color_window, cv::WINDOW_AUTOSIZE);
	cv::namedWindow(d1_depth_window, cv::WINDOW_AUTOSIZE);
	cv::namedWindow(d2_color_window, cv::WINDOW_AUTOSIZE);
	cv::namedWindow(d2_depth_window, cv::WINDOW_AUTOSIZE);

	int key;

	while (1)
	{
		for (auto dev : devices)
		{
			dev->poll_for_frames();
			const auto c = dev->get_stream_intrinsics(colorStream), d = dev->get_stream_intrinsics(rs::stream::depth);

			cv::Mat colorMat(dev->get_stream_height(colorStream), dev->get_stream_width(colorStream), CV_8UC3, (void *)dev->get_frame_data(colorStream));
			cv::Mat depthMat(dev->get_stream_height(depthStream), dev->get_stream_width(depthStream), CV_16UC1, (void *)dev->get_frame_data(depthStream));
			//cv::Mat depthMat(dev->get_stream_height(irstream), dev->get_stream_width(irstream), CV_16UC2, (void *)dev->get_frame_data(irstream));

			if (dev == devices.at(0)) {
				cv::imshow(d1_color_window, colorMat);
				cv::imshow(d1_depth_window, depthMat);
			}
			else {
				cv::imshow(d2_color_window, colorMat);
				cv::imshow(d2_depth_window, depthMat);
			}

			key = cv::waitKey(10);
			if (key == 27) {
				break;
			}
		}
	}

	return EXIT_SUCCESS;
}
catch (const rs::error & e)
{
	std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
	return EXIT_FAILURE;
}
catch (const std::exception & e)
{
	std::cerr << e.what() << std::endl;
	return EXIT_FAILURE;
}