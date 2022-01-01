/******************************************************************************
 * Image Browser
 * Caleb Wallwork
 * CSCI 4150
 *****************************************************************************/

#include <iostream>
#include <iomanip>
#include <vector>
#include <assert.h>
#include <filesystem>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/utility.hpp>
#include "browser.hpp"
#ifdef _WIN32
#include <Windows.h>
#include <dirent.h>
#endif

int maxcols = 720;	//!< Default max number of columns to show
int maxrows = 1280;	//!< Default max number of rows to show


/******************************************************************************
 * \brief Display the specified image.
 *
 * Display the image specified in the argument.  This function ensures that the
 * image will fit in the windows, while maintaining the aspect ratio.
 *
 * @param [in] Image to be displayed
 * \return Key pressed by the user
 *****************************************************************************/

uchar display(const cv::Mat& img)
{
	float ratio = img.col / img.rows;			// downsampling ratio
	
	// Find out the dominant dimension from rows and columns.  This needs to be
	// determined to find out whether the image is to be scaled by rows or columns.

	float ratio1 = static_cast<float>(maxcols) / img.cols;
	float ratio2 = static_cast<float>(maxrows) / img.rows;
	ratio = ratio1 < ratio2 ? ratio1 : ratio2;

	// Prepare the affine transform.  OpenCV allows you to specify the affine
	// transform by two triangles.  The first triangle specifies the top left,
	// top right, and bottom left corners of the original image.  The second
	// triangle specifies the corresponding points of the destination image.
	// Since the aspect ratio is to be preserved, the same ratio is used to
	// multiply rows and columns to find the coordinate points.

	cv::Point2f srcTri[3], dstTri[3];
	srcTri[0] = cv::Point2f(0.f, 0.f);
	srcTri[1] = cv::Point2f(img.cols - 1.f, 0.f);
	srcTri[2] = cv::Point2f(0.f, img.rows - 1.f);

	dstTri[0] = cv::Point2f(0.0f, 0.0f);
	dstTri[1] = cv::Point2f(img.cols * ratio, 0.0f);
	dstTri[2] = cv::Point2f(0.0f, img.rows * ratio);

	cv::Mat warp_mat = cv::getAffineTransform(srcTri, dstTri);

	// create the downsampled image and warp the original image into
	// downsampled image using the affine transform.

	cv::Mat image = cv::Mat::zeros(static_cast<int>(ratio * img.rows), static_cast<int>(ratio * img.cols), img.type());
	cv::warpAffine(img, image, warp_mat, image.size());

	// Print the original image resolution on the terminal and display the image.
	// Wait for the user to press any key, capture that key into response and
	// return the pressed key.

	std::cout << "\t" << img.cols << "x" << img.rows << std::endl;
	cv::imshow("Browser", image);
	uchar response = cv::waitKey(0);			//!< Key pressed by the user when image is displayed.

	return (response);
}


/******************************************************************************
 * \brief Store files form dir into files vector
 *
 * Recursively reads directory and its subdirectories for files
 *
 * @param [in] Directory path name and files string vector
 *****************************************************************************/
void file_list(std::string dir, std::vector<std::string> files)
{
	for (const auto& file : std::filesystem::recursive_directory_iterator(dir))
	{
		files.push_back(file.path());
	}
}

int main( int argc, const char ** argv )
{
	try
	{
		// parse the command line arguments

		cv::CommandLineParser parser(argc, argv, keys);
		std::string dir = parser.get<std::string>(0);
		
		parser.about("Image Browser v1.0");
		if (parser.has("help") || dir.empty() )
		{
			parser.printMessage();
			return (1);
		}
			
		maxrows = parser.get<int>("rows");
		maxcols = parser.get<int>("cols");

#ifdef _WIN32
		if (maxrows == 0 || maxcols == 0)
		{
			maxcols = static_cast<int>(GetSystemMetrics(SM_CXSCREEN));
			maxrows = static_cast<int>(GetSystemMetrics(SM_CYSCREEN));
		}
#endif
		
		// Scan all files in the specified directory.  If there are subdirectories,
		// the files in there are scanned as well in depth-first order.

		std::vector<std::string> files;			// list of all files
		files = file_list(dir, files);

		assert(files.size() != 0);				// list of files is not empty

		// Create window in the top left corner of screen

		cv::namedWindow("Browser", cv::WINDOW_AUTOSIZE);
		cv::moveWindow("Browser", 0, 0);

		// Display each file in the list in order

		for (int i = 0; i < files.size(); i++)
		{
			cv::Mat img = cv::imread(files[i]);

			// if the file does not contain an image, remove it from the list.

			while (img.empty())
			{
				files.erase(files.begin() + i, files.begin() + i + 1);
				if (i == files.size())
					return (0);
				img = cv::imread(files[i]);
			}

			assert(!img.empty());

			// Print the index number and name of the file containing the image.

			std::cout << std::setw(5) << i << ". " << std::setw(60) << files[i];

			uchar response = display(img);		// user response (valid values: q, n, p, space)

			if (response == 'q')				// user pressed q; quit
				break;

			if (response == 'n' || response == ' ') // user pressed n or space; display next image
				continue;

			if (response == 'p')				// user pressed p; display previous image
			{
				if (i == 0)
				{
					i--;
				}
				else
				{
					i -= 2;
				}
				continue;
			}
		}

		cv::destroyAllWindows();			// all done, remove the display window
	}
	catch (std::string& str)				// Handle string exception
	{
		std::cerr << "Error: " << argv[0] << ": " << str << std::endl;
		return (1);
	}
	catch (cv::Exception& e)				// Handle OpenCV exception
	{
		std::cerr << "Error: " << argv[0] << ": " << e.msg << std::endl;
		return (1);
	}
	return (0);
}