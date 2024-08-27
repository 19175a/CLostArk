#pragma once
#include "opencv2/core.hpp"
#include "ocrlite/ocrlite_struct.h"

#define OCRLITE_VERSION "1.2.0"
namespace ocrlite{
	namespace details {
		bool isFileExists(const std::string& name);
	}

	class DetDBNet;
	class RecCrnnNet;
	class ClsAngleNet;
	class OcrLite {
	public:
		OcrLite();

		~OcrLite();

		void setNumThread(int numOfThread);

		void initLogger(bool isConsole, bool isPartImg, bool isResultImg);

		void enableResultTxt(const char* path, const char* imgName);

		void setGpuIndex(int gpuIndex);

		bool initModels(const std::string& detPath, const std::string& clsPath,
			const std::string& recPath, const std::string& keysPath);

		void Logger(const char* format, ...);

		OcrResult detect(const char* path, const char* imgName,
			int padding, int maxSideLen,
			float boxScoreThresh, float boxThresh, float unClipRatio, bool doAngle, bool mostAngle);

		OcrResult detect(const cv::Mat& mat,
			int padding, int maxSideLen,
			float boxScoreThresh, float boxThresh, float unClipRatio, bool doAngle, bool mostAngle);
	private:
		bool m_isOutputConsole = false;
		bool m_isOutputPartImg = false;
		bool m_isOutputResultTxt = false;
		bool m_isOutputResultImg = false;
		FILE* m_resultTxt;
		std::shared_ptr<DetDBNet>  m_detDBNet;
		std::shared_ptr<RecCrnnNet> m_recCrnnNet;
		std::shared_ptr<ClsAngleNet> m_clsAngleNet;
		std::vector<cv::Mat> getPartImages(cv::Mat& src, std::vector<TextBox>& textBoxes,
			const char* path, const char* imgName);

		OcrResult detectInternal(const char* path, const char* imgName,
			cv::Mat& originSrc, int padding, int maxSideLen, float boxScoreThresh = 0.6f, float boxThresh = 0.3f,
			float unClipRatio = 2.0f, bool doAngle = true, bool mostAngle = true);


	};

}
