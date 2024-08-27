#include "pch.h"
#include <stdarg.h> //windows&linux
#include <sstream>
#include <fstream>
#include <numeric>
#include <sys/stat.h>

#include "ncnn/net.h"

#include "opencv2/opencv.hpp"
#include "ocrlite/ocrlite.h"
#include "ocrlite/clipper.hpp"


#include "ch_ppocr_det.id.h"
#include "ch_ppocr_det.mem.h"

#include "ch_ppocr_rec.id.h"
#include "ch_ppocr_rec.mem.h"

#include "ch_ppocr_cls.id.h"
#include "ch_ppocr_cls.mem.h"

#include "ch_ppocr_keys.mem.h"
//https://blog.csdn.net/Enchanted_ZhouH/article/details/106063552

namespace ocrlite {

	namespace details {


		/*#define __ENABLE_CONSOLE__ true
		#define Logger(format, ...) {\
		  if(__ENABLE_CONSOLE__) printf(format,##__VA_ARGS__); \
		}*/
		template<class T>
		inline T clamp(T x, T min, T max) {
			if (x > max)
				return max;
			if (x < min)
				return min;
			return x;
		}

		double getCurrentTime();

		bool isFileExists(const std::string& name) {
			struct stat buffer;
			return (stat(name.c_str(), &buffer) == 0);
		}

		std::wstring strToWstr(std::string str);

		ScaleParam getScaleParam(cv::Mat& src, const float scale);

		ScaleParam getScaleParam(cv::Mat& src, const int targetSize);

		std::vector<cv::Point2f> getBox(const cv::RotatedRect& rect);

		int getThickness(cv::Mat& boxImg);

		void drawTextBox(cv::Mat& boxImg, cv::RotatedRect& rect, int thickness);

		void drawTextBox(cv::Mat& boxImg, const std::vector<cv::Point>& box, int thickness);

		void drawTextBoxes(cv::Mat& boxImg, std::vector<TextBox>& textBoxes, int thickness);

		cv::Mat matRotateClockWise180(cv::Mat src);

		cv::Mat matRotateClockWise90(cv::Mat src);

		cv::Mat getRotateCropImage(const cv::Mat& src, std::vector<cv::Point> box);

		cv::Mat adjustTargetImg(cv::Mat& src, int dstWidth, int dstHeight);

		std::vector<cv::Point2f> getMinBoxes(const cv::RotatedRect& boxRect, float& maxSideLen);

		float boxScoreFast(const std::vector<cv::Point2f>& boxes, const cv::Mat& pred);

		cv::RotatedRect unClip(std::vector<cv::Point2f> box, float unClipRatio);

		std::vector<int> getAngleIndexes(std::vector<Angle>& angles);

		void saveImg(cv::Mat& img, const char* imgPath);

		std::string getSrcImgFilePath(const char* path, const char* imgName);

		std::string getResultTxtFilePath(const char* path, const char* imgName);

		std::string getResultImgFilePath(const char* path, const char* imgName);

		std::string getDebugImgFilePath(const char* path, const char* imgName, int i, const char* tag);

		void printGpuInfo();


		double getCurrentTime() {
			return (static_cast<double>(cv::getTickCount())) / cv::getTickFrequency() * 1000;//单位毫秒
		}

		//onnxruntime init windows
		std::wstring strToWstr(std::string str) {
			if (str.length() == 0)
				return L"";
			std::wstring wstr;
			wstr.assign(str.begin(), str.end());
			return wstr;
		}

		ScaleParam getScaleParam(cv::Mat& src, const float scale) {
			int srcWidth = src.cols;
			int srcHeight = src.rows;
			int dstWidth = int((float)srcWidth * scale);
			int dstHeight = int((float)srcHeight * scale);
			if (dstWidth % 32 != 0) {
				dstWidth = (dstWidth / 32 - 1) * 32;
				dstWidth = (std::max)(dstWidth, 32);
			}
			if (dstHeight % 32 != 0) {
				dstHeight = (dstHeight / 32 - 1) * 32;
				dstHeight = (std::max)(dstHeight, 32);
			}
			float scaleWidth = (float)dstWidth / (float)srcWidth;
			float scaleHeight = (float)dstHeight / (float)srcHeight;
			return { srcWidth, srcHeight, dstWidth, dstHeight, scaleWidth, scaleHeight };
		}

		ScaleParam getScaleParam(cv::Mat& src, const int targetSize) {
			int srcWidth, srcHeight, dstWidth, dstHeight;
			srcWidth = dstWidth = src.cols;
			srcHeight = dstHeight = src.rows;

			float ratio = 1.f;
			if (srcWidth > srcHeight) {
				ratio = float(targetSize) / float(srcWidth);
			}
			else {
				ratio = float(targetSize) / float(srcHeight);
			}
			dstWidth = int(float(srcWidth) * ratio);
			dstHeight = int(float(srcHeight) * ratio);
			if (dstWidth % 32 != 0) {
				dstWidth = (dstWidth / 32) * 32;
				dstWidth = (std::max)(dstWidth, 32);
			}
			if (dstHeight % 32 != 0) {
				dstHeight = (dstHeight / 32) * 32;
				dstHeight = (std::max)(dstHeight, 32);
			}
			float ratioWidth = (float)dstWidth / (float)srcWidth;
			float ratioHeight = (float)dstHeight / (float)srcHeight;
			return { srcWidth, srcHeight, dstWidth, dstHeight, ratioWidth, ratioHeight };
		}

		std::vector<cv::Point2f> getBox(const cv::RotatedRect& rect) {
			cv::Point2f vertices[4];
			rect.points(vertices);
			//std::vector<cv::Point2f> ret(4);
			std::vector<cv::Point2f> ret2(vertices, vertices + sizeof(vertices) / sizeof(vertices[0]));
			//memcpy(vertices, &ret[0], ret.size() * sizeof(ret[0]));
			return ret2;
		}

		/// <summary>
		/// 获取线的厚度
		/// </summary>
		/// <param name="boxImg"></param>
		/// <returns></returns>
		int getThickness(cv::Mat& boxImg) {
			int minSize = boxImg.cols > boxImg.rows ? boxImg.rows : boxImg.cols;
			int thickness = minSize / 1000 + 2;
			return thickness;
		}

		/// <summary>
		/// 通过矩形参数绘制单行框
		/// </summary>
		/// <param name="boxImg"></param>
		/// <param name="rect"></param>
		/// <param name="thickness"></param>
		void drawTextBox(cv::Mat& boxImg, cv::RotatedRect& rect, int thickness) {
			cv::Point2f vertices[4];
			rect.points(vertices);
			for (int i = 0; i < 4; i++)
				cv::line(boxImg, vertices[i], vertices[(i + 1) % 4], cv::Scalar(0, 0, 255), thickness);
			//cv::polylines(srcmat, textpoint, true, cv::Scalar(0, 255, 0), 2);
		}

		/// <summary>
		/// 通过四个角点绘制单行框
		/// </summary>
		/// <param name="boxImg"></param>
		/// <param name="box"></param>
		/// <param name="thickness"></param>
		void drawTextBox(cv::Mat& boxImg, const std::vector<cv::Point>& box, int thickness) {
			auto color = cv::Scalar(0, 0, 255);// B(0) G(0) R(255)
			cv::line(boxImg, box[0], box[1], color, thickness);
			cv::line(boxImg, box[1], box[2], color, thickness);
			cv::line(boxImg, box[2], box[3], color, thickness);
			cv::line(boxImg, box[3], box[0], color, thickness);
		}

		/// <summary>
		/// 通过检测到的文本对象绘制单行框
		/// </summary>
		/// <param name="boxImg"></param>
		/// <param name="box"></param>
		/// <param name="thickness"></param>
		void drawTextBoxes(cv::Mat& boxImg, std::vector<TextBox>& textBoxes, int thickness) {
			for (int i = 0; i < textBoxes.size(); ++i) {
				drawTextBox(boxImg, textBoxes[i].boxPoint, thickness);
			}
		}

		cv::Mat matRotateClockWise180(cv::Mat src) {
			flip(src, src, 0);
			flip(src, src, 1);
			return src;
		}

		cv::Mat matRotateClockWise90(cv::Mat src) {
			transpose(src, src);
			flip(src, src, 1);
			return src;
		}

		cv::Mat getRotateCropImage(const cv::Mat& src, std::vector<cv::Point> box) {
			cv::Mat image;
			src.copyTo(image);
			std::vector<cv::Point> points = box;

			int collectX[4] = { box[0].x, box[1].x, box[2].x, box[3].x };
			int collectY[4] = { box[0].y, box[1].y, box[2].y, box[3].y };
			int left = int(*std::min_element(collectX, collectX + 4));
			int right = int(*std::max_element(collectX, collectX + 4));
			int top = int(*std::min_element(collectY, collectY + 4));
			int bottom = int(*std::max_element(collectY, collectY + 4));

			cv::Mat imgCrop;
			image(cv::Rect(left, top, right - left, bottom - top)).copyTo(imgCrop);

			for (int i = 0; i < points.size(); i++) {
				points[i].x -= left;
				points[i].y -= top;
			}

			int imgCropWidth = int(sqrt(pow(points[0].x - points[1].x, 2) +
				pow(points[0].y - points[1].y, 2)));
			int imgCropHeight = int(sqrt(pow(points[0].x - points[3].x, 2) +
				pow(points[0].y - points[3].y, 2)));

			cv::Point2f ptsDst[4];
			ptsDst[0] = cv::Point2f(0., 0.);
			ptsDst[1] = cv::Point2f(imgCropWidth, 0.);
			ptsDst[2] = cv::Point2f(imgCropWidth, imgCropHeight);
			ptsDst[3] = cv::Point2f(0.f, imgCropHeight);

			cv::Point2f ptsSrc[4];
			ptsSrc[0] = cv::Point2f(points[0].x, points[0].y);
			ptsSrc[1] = cv::Point2f(points[1].x, points[1].y);
			ptsSrc[2] = cv::Point2f(points[2].x, points[2].y);
			ptsSrc[3] = cv::Point2f(points[3].x, points[3].y);

			cv::Mat M = cv::getPerspectiveTransform(ptsSrc, ptsDst);

			cv::Mat partImg;
			cv::warpPerspective(imgCrop, partImg, M,
				cv::Size(imgCropWidth, imgCropHeight),
				cv::BORDER_REPLICATE);

			if (float(partImg.rows) >= float(partImg.cols) * 1.5) {
				cv::Mat srcCopy = cv::Mat(partImg.rows, partImg.cols, partImg.depth());
				cv::transpose(partImg, srcCopy);
				cv::flip(srcCopy, srcCopy, 0);
				return srcCopy;
			}
			else {
				return partImg;
			}
		}

		cv::Mat adjustTargetImg(cv::Mat& src, int dstWidth, int dstHeight) {
			cv::Mat srcResize;
			float scale = (float)dstHeight / (float)src.rows;
			int angleWidth = int((float)src.cols * scale);
			cv::resize(src, srcResize, cv::Size(angleWidth, dstHeight));
			cv::Mat srcFit = cv::Mat(dstHeight, dstWidth, CV_8UC3, cv::Scalar(255, 255, 255));
			if (angleWidth < dstWidth) {
				cv::Rect rect(0, 0, srcResize.cols, srcResize.rows);
				srcResize.copyTo(srcFit(rect));
			}
			else {
				cv::Rect rect(0, 0, dstWidth, dstHeight);
				srcResize(rect).copyTo(srcFit);
			}
			return srcFit;
		}

		bool cvPointCompare(const cv::Point& a, const cv::Point& b) {
			return a.x < b.x;
		}

		std::vector<cv::Point2f> getMinBoxes(const cv::RotatedRect& boxRect, float& maxSideLen) {
			maxSideLen = (std::max)(boxRect.size.width, boxRect.size.height);
			std::vector<cv::Point2f> boxPoint = getBox(boxRect);
			std::sort(boxPoint.begin(), boxPoint.end(), cvPointCompare);
			int index1, index2, index3, index4;
			if (boxPoint[1].y > boxPoint[0].y) {
				index1 = 0;
				index4 = 1;
			}
			else {
				index1 = 1;
				index4 = 0;
			}
			if (boxPoint[3].y > boxPoint[2].y) {
				index2 = 2;
				index3 = 3;
			}
			else {
				index2 = 3;
				index3 = 2;
			}
			std::vector<cv::Point2f> minBox(4);
			minBox[0] = boxPoint[index1];
			minBox[1] = boxPoint[index2];
			minBox[2] = boxPoint[index3];
			minBox[3] = boxPoint[index4];
			return minBox;
		}

		float boxScoreFast(const std::vector<cv::Point2f>& boxes, const cv::Mat& pred) {
			int width = pred.cols;
			int height = pred.rows;

			float arrayX[4] = { boxes[0].x, boxes[1].x, boxes[2].x, boxes[3].x };
			float arrayY[4] = { boxes[0].y, boxes[1].y, boxes[2].y, boxes[3].y };

			int minX = clamp(int(std::floor(*(std::min_element(arrayX, arrayX + 4)))), 0, width - 1);
			int maxX = clamp(int(std::ceil(*(std::max_element(arrayX, arrayX + 4)))), 0, width - 1);
			int minY = clamp(int(std::floor(*(std::min_element(arrayY, arrayY + 4)))), 0, height - 1);
			int maxY = clamp(int(std::ceil(*(std::max_element(arrayY, arrayY + 4)))), 0, height - 1);

			cv::Mat mask = cv::Mat::zeros(maxY - minY + 1, maxX - minX + 1, CV_8UC1);

			cv::Point box[4];
			box[0] = cv::Point(int(boxes[0].x) - minX, int(boxes[0].y) - minY);
			box[1] = cv::Point(int(boxes[1].x) - minX, int(boxes[1].y) - minY);
			box[2] = cv::Point(int(boxes[2].x) - minX, int(boxes[2].y) - minY);
			box[3] = cv::Point(int(boxes[3].x) - minX, int(boxes[3].y) - minY);
			const cv::Point* pts[1] = { box };
			int npts[] = { 4 };
			cv::fillPoly(mask, pts, npts, 1, cv::Scalar(1));

			cv::Mat croppedImg;
			pred(cv::Rect(minX, minY, maxX - minX + 1, maxY - minY + 1))
				.copyTo(croppedImg);

			auto score = cv::mean(croppedImg, mask)[0];
			return score;
		}

		float getContourArea(const std::vector<cv::Point2f>& box, float unClipRatio) {
			int size = box.size();
			float area = 0.0f;
			float dist = 0.0f;
			for (int i = 0; i < size; i++) {
				area += box[i].x * box[(i + 1) % size].y -
					box[i].y * box[(i + 1) % size].x;
				dist += sqrtf((box[i].x - box[(i + 1) % size].x) *
					(box[i].x - box[(i + 1) % size].x) +
					(box[i].y - box[(i + 1) % size].y) *
					(box[i].y - box[(i + 1) % size].y));
			}
			area = fabs(float(area / 2.0));

			return area * unClipRatio / dist;
		}

		cv::RotatedRect unClip(std::vector<cv::Point2f> box, float unClipRatio) {
			float distance = getContourArea(box, unClipRatio);

			ClipperLib::ClipperOffset offset;
			ClipperLib::Path p;
			p << ClipperLib::IntPoint(int(box[0].x), int(box[0].y))
				<< ClipperLib::IntPoint(int(box[1].x), int(box[1].y))
				<< ClipperLib::IntPoint(int(box[2].x), int(box[2].y))
				<< ClipperLib::IntPoint(int(box[3].x), int(box[3].y));
			offset.AddPath(p, ClipperLib::jtRound, ClipperLib::etClosedPolygon);

			ClipperLib::Paths soln;
			offset.Execute(soln, distance);
			std::vector<cv::Point2f> points;

			for (int j = 0; j < soln.size(); j++) {
				for (int i = 0; i < soln[soln.size() - 1].size(); i++) {
					points.emplace_back(soln[j][i].X, soln[j][i].Y);
				}
			}
			cv::RotatedRect res;
			if (points.empty()) {
				res = cv::RotatedRect(cv::Point2f(0, 0), cv::Size2f(1, 1), 0);
			}
			else {
				res = cv::minAreaRect(points);
			}
			return res;
		}

		std::vector<int> getAngleIndexes(std::vector<Angle>& angles) {
			std::vector<int> angleIndexes;
			angleIndexes.reserve(angles.size());
			for (int i = 0; i < angles.size(); ++i) {
				angleIndexes.push_back(angles[i].index);
			}
			return angleIndexes;
		}

		void saveImg(cv::Mat& img, const char* imgPath) {
			cv::imwrite(imgPath, img);
		}

		std::string getSrcImgFilePath(const char* path, const char* imgName) {
			std::string filePath;
			filePath.append(path).append(imgName);
			return filePath;
		}

		std::string getResultTxtFilePath(const char* path, const char* imgName) {
			std::string filePath;
			filePath.append(path).append(imgName).append("-result.txt");
			return filePath;
		}

		std::string getResultImgFilePath(const char* path, const char* imgName) {
			std::string filePath;
			filePath.append(path).append(imgName).append("-result.jpg");
			return filePath;
		}

		std::string getDebugImgFilePath(const char* path, const char* imgName, int i, const char* tag) {
			std::string filePath;
			filePath.append(path).append(imgName).append(tag).append(std::to_string(i)).append(".jpg");
			return filePath;
		}

		void printGpuInfo() {
#ifdef __VULKAN__
			auto gpuCount = ncnn::get_gpu_count();
			if (gpuCount != 0) {
				printf("This device has %d GPUs\n", gpuCount);
			}
			else {
				printf("This device does not have a GPU\n");
			}
#endif
		}

		template<class ForwardIterator>
		inline static size_t argmax(ForwardIterator first, ForwardIterator last) {
			return std::distance(first, std::max_element(first, last));
		}

		Angle scoreToAngle(const std::vector<float>& outputData) {
			int maxIndex = 0;
			float maxScore = 0;
			for (int i = 0; i < outputData.size(); i++) {
				if (outputData[i] > maxScore) {
					maxScore = outputData[i];
					maxIndex = i;
				}
			}
			return { maxIndex, maxScore };
		}
		cv::Mat makePadding(cv::Mat& src, const int padding) {
			if (padding <= 0) return src;
			cv::Scalar paddingScalar = { 255, 255, 255 };
			cv::Mat paddingSrc;
			cv::copyMakeBorder(src, paddingSrc, padding, padding, padding, padding, cv::BORDER_ISOLATED, paddingScalar);
			return paddingSrc;
		}

	}

	class NcnnNetWarpper {
	public:
		NcnnNetWarpper() = default;
		virtual ~NcnnNetWarpper() {
			m_net.clear();
		}
		void setNumThread(int numOfThread) {
			m_net.opt.num_threads = numOfThread;
			m_numThread = numOfThread;
		}
		void setGpuIndex(int gpuIndex) {
#ifdef __VULKAN__
			if (gpuIndex >= 0) {
				m_net.opt.use_vulkan_compute = true;
				m_net.set_vulkan_device(gpuIndex);
				printf("dbNet try to use Gpu%d\n", gpuIndex);
			}
			else {
				m_net.opt.use_vulkan_compute = false;
				printf("dbNet use Cpu\n");
			}
#endif
		}
	protected:
		int m_numThread;
		ncnn::Net m_net;
	};

	class DetDBNet final : public NcnnNetWarpper {
	public:
		bool initModel(const std::string& pathStr) {
			if (0) {
				int dbParam = m_net.load_param((pathStr + ".param").c_str());
				int dbModel = m_net.load_model((pathStr + ".bin").c_str());
				if (dbParam != 0 || dbModel != 0) {
					printf("DBNet load param(%d), model(%d)\n", dbParam, dbModel);
					return false;
				}
				else {
					return true;
				}
			}
			else {
				int ret_param = m_net.load_param(ch_PP_OCRv3_det_infer_param_bin);
				int ret_bin = m_net.load_model(ch_PP_OCRv3_det_infer_bin);
				if (ret_param != sizeof(ch_PP_OCRv3_det_infer_param_bin) || ret_bin != sizeof(ch_PP_OCRv3_det_infer_bin)) {
					printf("AngleNet load param(%d), model(%d)\n", ret_param, ret_bin);
					return false;
				}
				else {
					return true;
				}
			}
			return false;
		}

		std::vector<TextBox> findRsBoxes(const cv::Mat& predMat, const cv::Mat& dilateMat, ScaleParam& s,
			const float boxScoreThresh, const float unClipRatio) {
			const int longSideThresh = 3;//minBox 长边门限
			const int maxCandidates = 1000;

			std::vector<std::vector<cv::Point>> contours;
			std::vector<cv::Vec4i> hierarchy;

			cv::findContours(dilateMat, contours, hierarchy, cv::RETR_LIST,
				cv::CHAIN_APPROX_SIMPLE);

			int numContours = contours.size() >= maxCandidates ? maxCandidates : contours.size();

			std::vector<TextBox> rsBoxes;

			for (int i = 0; i < numContours; i++) {
				if (contours[i].size() <= 2) {
					continue;
				}
				cv::RotatedRect minAreaRect = cv::minAreaRect(contours[i]);

				float longSide;
				std::vector<cv::Point2f> minBoxes = details::getMinBoxes(minAreaRect, longSide);

				if (longSide < longSideThresh) {
					continue;
				}

				float boxScore = details::boxScoreFast(minBoxes, predMat);
				if (boxScore < boxScoreThresh)
					continue;

				//-----unClip-----
				cv::RotatedRect clipRect = details::unClip(minBoxes, unClipRatio);
				if (clipRect.size.height < 1.001 && clipRect.size.width < 1.001) {
					continue;
				}
				//-----unClip-----

				std::vector<cv::Point2f> clipMinBoxes = details::getMinBoxes(clipRect, longSide);
				if (longSide < longSideThresh + 2)
					continue;

				std::vector<cv::Point> intClipMinBoxes;

				for (int p = 0; p < clipMinBoxes.size(); p++) {
					float x = clipMinBoxes[p].x / s.ratioWidth;
					float y = clipMinBoxes[p].y / s.ratioHeight;
					int ptX = (std::min)((std::max)(int(x), 0), s.srcWidth - 1);
					int ptY = (std::min)((std::max)(int(y), 0), s.srcHeight - 1);
					cv::Point point{ptX, ptY};
					intClipMinBoxes.push_back(point);
				}
				rsBoxes.push_back(TextBox{ intClipMinBoxes, boxScore });
			}
			reverse(rsBoxes.begin(), rsBoxes.end());
			return rsBoxes;
		}

		std::vector<TextBox> getTextBoxes(cv::Mat& src, ScaleParam& s, float boxScoreThresh, float boxThresh, float unClipRatio) {
			cv::Mat srcResize;
			cv::resize(src, srcResize, cv::Size(s.dstWidth, s.dstHeight));
			ncnn::Mat input = ncnn::Mat::from_pixels(srcResize.data, ncnn::Mat::PIXEL_RGB,
				srcResize.cols, srcResize.rows);
			ncnn::Mat out;

			input.substract_mean_normalize(meanValues, normValues);
			ncnn::Extractor extractor = m_net.create_extractor();
			//extractor.set_num_threads(m_numThread);

			if (0) {
				extractor.input("input", input);
				extractor.extract("output", out);
			}
			else {
				extractor.input(ch_PP_OCRv3_det_infer_param_id::BLOB_input, input);
				extractor.extract(ch_PP_OCRv3_det_infer_param_id::BLOB_output, out);
			}

			//-----Data preparation-----
			cv::Mat fMapMat(srcResize.rows, srcResize.cols, CV_32FC1);
			memcpy(fMapMat.data, (float*)out.data, srcResize.rows * srcResize.cols * sizeof(float));

			//-----boxThresh-----
			cv::Mat norfMapMat;
			norfMapMat = fMapMat > boxThresh;
			return findRsBoxes(fMapMat, norfMapMat, s, boxScoreThresh, unClipRatio);
		}

	private:

		const float meanValues[3] = { 0.485 * 255, 0.456 * 255, 0.406 * 255 };
		const float normValues[3] = { 1.0 / 0.229 / 255.0, 1.0 / 0.224 / 255.0, 1.0 / 0.225 / 255.0 };
	};

	class RecCrnnNet final : public NcnnNetWarpper {
	public:
		bool initModel(const std::string& pathStr, const std::string& keysPath) {
			if (0) {

				int ret_param = m_net.load_param((pathStr + ".param").c_str());
				int ret_bin = m_net.load_model((pathStr + ".bin").c_str());
				if (ret_param != 0 || ret_bin != 0) {
					printf("CrnnNet load param(%d), model(%d)\n", ret_param, ret_bin);
					return false;
				}
				//load keys
				std::ifstream in(keysPath.c_str());
				std::string line;
				if (in.is_open()) {
					while (getline(in, line)) {// line中不包括每行的换行符
						m_keys.push_back(line);
					}
				}
				else {
					printf("The keys.txt file was not found\n");
					return false;
				}
				m_keys.insert(m_keys.begin(), "#");
				m_keys.emplace_back(" ");
				printf("total keys size(%lu)\n", m_keys.size());
			}
			else {
				int ret_param = m_net.load_param(ch_PP_OCRv3_rec_infer_param_bin);
				int ret_bin = m_net.load_model(ch_PP_OCRv3_rec_infer_bin);
				if (ret_param != sizeof(ch_PP_OCRv3_rec_infer_param_bin) || ret_bin != sizeof(ch_PP_OCRv3_rec_infer_bin)) {
					printf("CrnnNet load param(%d), model(%d)\n", ret_param, ret_bin);
					return false;
				}
				std::string line;
				std::stringstream ss;
				ss << std::string(ch_ppocr_keys_bin, ch_ppocr_keys_bin + sizeof(ch_ppocr_keys_bin));
				while (getline(ss, line)) {// line中不包括每行的换行符
					m_keys.push_back(line);
				}
				m_keys.insert(m_keys.begin(), "#");
				m_keys.emplace_back(" ");
				return true;
			}
		}

		std::vector<TextLine> getTextLines(std::vector<cv::Mat>& partImg, const char* path, const char* imgName) {
			int size = partImg.size();
			std::vector<TextLine> textLines(size);
			for (int i = 0; i < size; ++i) {
				//OutPut DebugImg
				if (m_isOutputDebugImg) {
					std::string debugImgFile = details::getDebugImgFilePath(path, imgName, i, "-debug-");
					details::saveImg(partImg[i], debugImgFile.c_str());
				}

				//getTextLine
				double startCrnnTime = details::getCurrentTime();
				TextLine textLine = getTextLine(partImg[i]);
				double endCrnnTime = details::getCurrentTime();
				textLine.time = endCrnnTime - startCrnnTime;
				textLines[i] = textLine;
			}
			return textLines;
		}

	private:
		bool m_isOutputDebugImg = false;
		const float m_meanValues[3] = { 127.5, 127.5, 127.5 };
		const float m_normValues[3] = { 1.0 / 127.5, 1.0 / 127.5, 1.0 / 127.5 };
		const int m_dstHeight = 48;

		std::vector<std::string> m_keys;

		TextLine scoreToTextLine(const std::vector<float>& outputData, int h, int w) {
			auto keySize = m_keys.size();
			auto dataSize = outputData.size();
			std::string strRes;
			std::vector<float> scores;
			int lastIndex = 0;
			int maxIndex;
			float maxValue;

			for (int i = 0; i < h; i++) {
				int start = i * w;
				int stop = (i + 1) * w;
				if (stop > dataSize - 1) {
					stop = (i + 1) * w - 1;
				}
				maxIndex = int(details::argmax(&outputData[start], &outputData[stop]));
				maxValue = float(*std::max_element(&outputData[start], &outputData[stop]));

				if (maxIndex > 0 && maxIndex < keySize && (!(i > 0 && maxIndex == lastIndex))) {
					scores.emplace_back(maxValue);
					strRes.append(m_keys[maxIndex]);
				}
				lastIndex = maxIndex;
			}
			return { strRes, scores };
		}

		TextLine getTextLine(const cv::Mat& src) {
			float scale = (float)m_dstHeight / (float)src.rows;
			int dstWidth = int((float)src.cols * scale);

			cv::Mat srcResize;
			resize(src, srcResize, cv::Size(dstWidth, m_dstHeight));

			ncnn::Mat input = ncnn::Mat::from_pixels(
				srcResize.data, ncnn::Mat::PIXEL_RGB,
				srcResize.cols, srcResize.rows);
			ncnn::Mat out;
			input.substract_mean_normalize(m_meanValues, m_normValues);

			ncnn::Extractor extractor = m_net.create_extractor();
			//extractor.set_num_threads(m_numThread);
			if (0) {
				extractor.input("input", input);
				extractor.extract("output", out);
			}
			else {
				extractor.input(ch_PP_OCRv3_rec_infer_param_id::BLOB_input, input);
				extractor.extract(ch_PP_OCRv3_rec_infer_param_id::BLOB_output, out);
			}
			float* floatArray = (float*)out.data;
			std::vector<float> outputData(floatArray, floatArray + out.h * out.w);
			return scoreToTextLine(outputData, out.h, out.w);
		}
	};

	class ClsAngleNet final : public NcnnNetWarpper {
	public:
		bool initModel(const std::string& pathStr) 
		{
			if (0) {
				int ret_param = m_net.load_param((pathStr + ".param").c_str());
				int ret_bin = m_net.load_model((pathStr + ".bin").c_str());
				if (ret_param != 0 || ret_bin != 0) {
					printf("AngleNet load param(%d), model(%d)\n", ret_param, ret_bin);
					return false;
				}
				else {
					return true;
				}
			}
			else {
				int ret_param = m_net.load_param(ch_ppocr_mobile_v2_0_cls_infer_param_bin);
				int ret_bin = m_net.load_model(ch_ppocr_mobile_v2_0_cls_infer_bin);
				if (ret_param != sizeof(ch_ppocr_mobile_v2_0_cls_infer_param_bin) || ret_bin != sizeof(ch_ppocr_mobile_v2_0_cls_infer_bin)) {
					printf("AngleNet load param(%d), model(%d)\n", ret_param, ret_bin);
					return false;
				}
				else {
					return true;
				}

			}
			return false;

		}

		std::vector<Angle> getAngles(std::vector<cv::Mat>& partImgs, const char* path, 
			const char* imgName,
			bool doAngle, 
			bool mostAngle) 
		{

			int size = partImgs.size();
			std::vector<Angle> angles(size);
		if (doAngle) {
			for (int i = 0; i < size; ++i) {
				double startAngle = details::getCurrentTime();
				cv::Mat angleImg;
				cv::resize(partImgs[i], angleImg, cv::Size(m_dstWidth, m_dstHeight));
				Angle angle = getAngle(angleImg);
				double endAngle = details::getCurrentTime();
				angle.time = endAngle - startAngle;

				angles[i] = angle;

				//OutPut AngleImg
				if (m_isOutputAngleImg) {
					std::string angleImgFile = details::getDebugImgFilePath(path, imgName, i, "-angle-");
					details::saveImg(angleImg, angleImgFile.c_str());
				}
			}
		}
		else {
			for (int i = 0; i < size; ++i) {
				angles[i] = Angle{ -1, 0.f };
			}
		}
		//Most Possible AngleIndex
		if (doAngle && mostAngle) {
			auto angleIndexes = details::getAngleIndexes(angles);
			double sum = std::accumulate(angleIndexes.begin(), angleIndexes.end(), 0.0);
			double halfPercent = angles.size() / 2.0f;
			int mostAngleIndex;
			if (sum < halfPercent) {//all angle set to 0
				mostAngleIndex = 0;
			}
			else {//all angle set to 1
				mostAngleIndex = 1;
			}
			//printf("Set All Angle to mostAngleIndex(%d)\n", mostAngleIndex);
			for (int i = 0; i < angles.size(); ++i) {
				Angle angle = angles[i];
				angle.index = mostAngleIndex;
				angles.at(i) = angle;
			}
		}

		return angles;
	}

private:
	bool m_isOutputAngleImg = false;
	const float m_meanValues[3] = { 127.5, 127.5, 127.5 };
	const float m_normValues[3] = { 1.0 / 127.5, 1.0 / 127.5, 1.0 / 127.5 };

	const int m_dstWidth = 192;
	const int m_dstHeight = 48;

	Angle getAngle(cv::Mat& src) {
			ncnn::Mat input = ncnn::Mat::from_pixels(
				src.data, ncnn::Mat::PIXEL_RGB,
				src.cols, src.rows);
			ncnn::Mat out;

			input.substract_mean_normalize(m_meanValues, m_normValues);
			ncnn::Extractor extractor = m_net.create_extractor();
			//extractor.set_num_threads(m_numThread);
			if (0) {
				extractor.input("input", input);
				extractor.extract("output", out);
			}
			else {
				extractor.input(ch_ppocr_mobile_v2_0_cls_infer_param_id::BLOB_input, input);
				extractor.extract(ch_ppocr_mobile_v2_0_cls_infer_param_id::BLOB_output, out);
			}

			float* floatArray = (float*)out.data;
			std::vector<float> outputData(floatArray, floatArray + out.w);
			return details::scoreToAngle(outputData);
		}
	};

	OcrLite::OcrLite():
		m_detDBNet(new DetDBNet()),
		m_clsAngleNet(new ClsAngleNet()),
		m_recCrnnNet(new RecCrnnNet())
	{

	}

	OcrLite::~OcrLite() {
		if (m_isOutputResultTxt) {
			fclose(m_resultTxt);
		}
	}

	void OcrLite::setNumThread(int numOfThread) {
		m_detDBNet->setNumThread(numOfThread);
		m_clsAngleNet->setNumThread(numOfThread);
		m_recCrnnNet->setNumThread(numOfThread);
	}

	void OcrLite::initLogger(bool isConsole, bool isPartImg, bool isResultImg) {
		m_isOutputConsole = isConsole;
		m_isOutputPartImg = isPartImg;
		m_isOutputResultImg = isResultImg;
	}

	void OcrLite::enableResultTxt(const char* path, const char* imgName) {
		m_isOutputResultTxt = true;
		std::string resultTxtPath = details::getResultTxtFilePath(path, imgName);
		printf("resultTxtPath(%s)\n", resultTxtPath.c_str());
		m_resultTxt = fopen(resultTxtPath.c_str(), "w");
	}

	void OcrLite::setGpuIndex(int gpuIndex) {
		m_detDBNet->setGpuIndex(gpuIndex);
		m_clsAngleNet->setGpuIndex(-1);
		m_recCrnnNet->setGpuIndex(gpuIndex);
	}

	bool OcrLite::initModels(const std::string& detPath, const std::string& clsPath,
		const std::string& recPath, const std::string& keysPath) {
		Logger("=====Init Models=====\n");
		Logger("--- Init DbNet ---\n");
		bool retDbNet = m_detDBNet->initModel(detPath);

		Logger("--- Init AngleNet ---\n");
		bool retAngleNet = m_clsAngleNet->initModel(clsPath);

		Logger("--- Init CrnnNet ---\n");
		bool retCrnnNet = m_recCrnnNet->initModel(recPath, keysPath);

		if (!retDbNet || !retAngleNet || !retCrnnNet) {
			Logger("Init Models Failed! %d  %d  %d\n", retDbNet, retAngleNet, retCrnnNet);
			return false;
		}
		return true;
	}

	void OcrLite::Logger(const char* format, ...) {
		if (!(m_isOutputConsole || m_isOutputResultTxt)) return;
		char* buffer = (char*)malloc(8192);
		va_list args;
		va_start(args, format);
		vsprintf(buffer, format, args);
		va_end(args);
		if (m_isOutputConsole) printf("%s", buffer);
		if (m_isOutputResultTxt) fprintf(m_resultTxt, "%s", buffer);
		free(buffer);
	}

	std::vector<cv::Mat> OcrLite::getPartImages(cv::Mat& src, std::vector<TextBox>& textBoxes,
		const char* path, const char* imgName) {
		std::vector<cv::Mat> partImages;
		for (int i = 0; i < textBoxes.size(); ++i) {
			cv::Mat partImg = details::getRotateCropImage(src, textBoxes[i].boxPoint);
			partImages.emplace_back(partImg);
			//OutPut DebugImg
			if (m_isOutputPartImg) {
				std::string debugImgFile = details::getDebugImgFilePath(path, imgName, i, "-part-");
				details::saveImg(partImg, debugImgFile.c_str());
			}
		}
		return partImages;
	}

	OcrResult OcrLite::detect(const char* path, const char* imgName,
		const int padding, const int maxSideLen,
		float boxScoreThresh, float boxThresh, float unClipRatio, bool doAngle, bool mostAngle) {
		std::string imgFile = details::getSrcImgFilePath(path, imgName);
		cv::Mat originSrc = imread(imgFile, cv::IMREAD_COLOR);//default : BGR
		OcrResult result;
		result = detectInternal(path, imgName, originSrc, padding, maxSideLen, boxScoreThresh, boxThresh, unClipRatio, doAngle, mostAngle);
		return result;
	}

	OcrResult OcrLite::detect(const cv::Mat& mat, int padding, int maxSideLen, float boxScoreThresh, float boxThresh, float unClipRatio, bool doAngle, bool mostAngle)
	{
		cv::Mat originSrc = mat;
		OcrResult result;
		result = detectInternal(NULL, NULL, originSrc, padding, maxSideLen,boxScoreThresh, boxThresh, unClipRatio, doAngle, mostAngle);
		return result;
	}


	OcrResult OcrLite::detectInternal(const char* path, const char* imgName,cv::Mat& originSrc, int padding, int maxSideLen,
		float boxScoreThresh, float boxThresh, float unClipRatio, bool doAngle, bool mostAngle) {
		cv::Mat src = {};
		cv::Rect originRect = {};
		ScaleParam scale = {};
		{
			int originMaxSide = (std::max)(originSrc.cols, originSrc.rows);
			int resize;
			if (maxSideLen <= 0 || maxSideLen > originMaxSide) {
				resize = originMaxSide;
			}
			else {
				resize = maxSideLen;
			}
			resize += 2 * padding;
			cv::Rect paddingRect(padding, padding, originSrc.cols, originSrc.rows);
			cv::Mat paddingSrc = details::makePadding(originSrc, padding);
			scale = details::getScaleParam(paddingSrc, resize);
			src = paddingSrc;
			originRect = paddingRect;
		}

		cv::Mat textBoxPaddingImg = src.clone();
		int thickness = details::getThickness(src);

		Logger("=====Start detect=====\n");
		Logger("ScaleParam(sw:%d,sh:%d,dw:%d,dh:%d,%f,%f)\n", scale.srcWidth, scale.srcHeight,
			scale.dstWidth, scale.dstHeight,
			scale.ratioWidth, scale.ratioHeight);
		Logger("---------- step: dbNet getTextBoxes ----------\n");
		double startTime = details::getCurrentTime();
		std::vector<TextBox> textBoxes = m_detDBNet->getTextBoxes(src, scale, boxScoreThresh, boxThresh, unClipRatio);
		double endDbNetTime = details::getCurrentTime();
		double dbNetTime = endDbNetTime - startTime;
		Logger("dbNetTime(%fms)\n", dbNetTime);

		//for (int i = 0; i < textBoxes.size(); ++i) {
		//	Logger("TextBox[%d](+padding)[score(%f),[x: %d, y: %d], [x: %d, y: %d], [x: %d, y: %d], [x: %d, y: %d]]\n", i,
		//		textBoxes[i].score,
		//		textBoxes[i].boxPoint[0].x, textBoxes[i].boxPoint[0].y,
		//		textBoxes[i].boxPoint[1].x, textBoxes[i].boxPoint[1].y,
		//		textBoxes[i].boxPoint[2].x, textBoxes[i].boxPoint[2].y,
		//		textBoxes[i].boxPoint[3].x, textBoxes[i].boxPoint[3].y);
		//}

		Logger("---------- step: drawTextBoxes ----------\n");
		//details::drawTextBoxes(textBoxPaddingImg, textBoxes, thickness);
		//test
		//cv::imshow("textBoxPaddingImg", textBoxPaddingImg);
		//cv::waitKey();
		//---------- getPartImages ----------
		std::vector<cv::Mat> partImages = getPartImages(src, textBoxes, path, imgName);

		Logger("---------- step: angleNet getAngles ----------\n");
		std::vector<Angle> angles;
		angles = m_clsAngleNet->getAngles(partImages, path, imgName, doAngle, mostAngle);

		//Log Angles
		//for (int i = 0; i < angles.size(); ++i) {
		//	Logger("angle[%d][index(%d), score(%f), time(%fms)]\n", i, angles[i].index, angles[i].score, angles[i].time);
		//}

		//Rotate partImgs
		for (int i = 0; i < partImages.size(); ++i) {
			if (angles[i].index == 1) {
				partImages.at(i) = details::matRotateClockWise180(partImages[i]);
			}
		}

		Logger("---------- step: crnnNet getTextLine ----------\n");
		std::vector<TextLine> textLines = m_recCrnnNet->getTextLines(partImages, path, imgName);
		//Log TextLines
		//for (int i = 0; i < textLines.size(); ++i) {
		//	Logger("textLine[%d](%s)\n", i, textLines[i].text.c_str());
		//	std::ostringstream txtScores;
		//	for (int s = 0; s < textLines[i].charScores.size(); ++s) {
		//		if (s == 0) {
		//			txtScores << textLines[i].charScores[s];
		//		}
		//		else {
		//			txtScores << " ," << textLines[i].charScores[s];
		//		}
		//	}
		//	Logger("textScores[%d]{%s}\n", i, std::string(txtScores.str()).c_str());
		//	Logger("crnnTime[%d](%fms)\n", i, textLines[i].time);
		//}

		std::vector<TextBlock> textBlocks;
		for (int i = 0; i < textLines.size(); ++i) {
			std::vector<cv::Point> boxPoint = std::vector<cv::Point>(4);
			int padding = originRect.x;//padding conversion
			boxPoint[0] = cv::Point(textBoxes[i].boxPoint[0].x - padding, textBoxes[i].boxPoint[0].y - padding);
			boxPoint[1] = cv::Point(textBoxes[i].boxPoint[1].x - padding, textBoxes[i].boxPoint[1].y - padding);
			boxPoint[2] = cv::Point(textBoxes[i].boxPoint[2].x - padding, textBoxes[i].boxPoint[2].y - padding);
			boxPoint[3] = cv::Point(textBoxes[i].boxPoint[3].x - padding, textBoxes[i].boxPoint[3].y - padding);
			TextBlock textBlock{ boxPoint, textBoxes[i].score, angles[i].index, angles[i].score,
								angles[i].time, textLines[i].text, textLines[i].charScores, textLines[i].time,
								angles[i].time + textLines[i].time };
			textBlocks.emplace_back(textBlock);
		}

		double endTime = details::getCurrentTime();
		double fullTime = endTime - startTime;
		Logger("=====End detect=====\n");
		Logger("FullDetectTime(%fms)\n", fullTime);

		//cropped to original size
		cv::Mat textBoxImg;

		if (originRect.x > 0 && originRect.y > 0) {
			textBoxPaddingImg(originRect).copyTo(textBoxImg);
		}
		else {
			textBoxImg = textBoxPaddingImg;
		}

		//Save result.jpg
		if (m_isOutputResultImg) {
			std::string resultImgFile = details::getResultImgFilePath(path, imgName);
			imwrite(resultImgFile, textBoxImg);
		}

		std::string strRes;
		for (int i = 0; i < textBlocks.size(); ++i) {
			strRes.append(textBlocks[i].text);
			strRes.append("\n");
		}

		return OcrResult{ dbNetTime, textBlocks, textBoxImg, fullTime, strRes };
	}

}
