
#include<iostream> // std::cout, std::cin
#include<string> // std::string
#include<itkImageFileReader.h>
#include<itkImageFileWriter.h>
#include<itkImageSeriesReader.h>
#include<itkImageSeriesWriter.h>
#include<itkHoughTransform2DCirclesImageFilter.h>
#include<itkThresholdImageFilter.h>
#include<itkImageRegionIterator.h>
#include<itkMinimumMaximumImageCalculator.h>
#include<itkGradientMagnitudeImageFilter.h>
#include<itkDiscreteGaussianImageFilter.h>
#include<itkCastImageFilter.h>
#include<itkEllipseSpatialObject.h>
#include<itkMath.h>
#include<itkGDCMImageIO.h>
#include<itkGDCMSeriesFileNames.h>
#include<itkNumericSeriesFileNames.h>

int main()
{
	/*Pixel Types*/
	typedef unsigned char PixelType;
	typedef float AccumulatorPixelType;
	typedef float RadiusPixelType;
	typedef signed short PixelType3D;

	/*Dimensions*/
	const unsigned int Dimension = 2;
	const int Dimension3D = 3;

	/*Image Types*/
	typedef itk::Image< PixelType, Dimension > ImageType;
	typedef itk::Image<PixelType3D, Dimension3D> ImageType3D;
	typedef itk::Image< AccumulatorPixelType, Dimension > AccumulatorImageType;

	/*Hough Transform Consts*/
	constexpr int numberOfCircles = 2;
	constexpr int radiusMin = 15;
	constexpr int radiusMax = 30;
	constexpr int sweepAngle = 0;
	constexpr int sigmaGradient = 1;
	constexpr int varianceOfAccumulatorBlurring = 5;
	constexpr int radiusOfTheDiskToRemoveFromTheAccumulator = 10;
	ImageType::IndexType localIndex;

	/*Paths*/
	char* inputFileName = "Images/Input/Gsp12_Gsp12/Head_Routine - 20090514/T1_SE_TRA_PAT2_FIL_4/IM-0002-0004.dcm";
	char* outputFileName = "Images/Output/IM-0002-0004-C.dcm";
	char* inputSeriesPath = "Images/Input/Gsp12_Gsp12/Head_Routine - 20090514/T1_SE_TRA_PAT2_FIL_4/";
	char* outputSeriesNames = "Images/Output/ReadSeries/IMG%05d.dcm";

	//////////////////////////////////////////2D////////////////////////////////////////////
	////ODCZYTYWANIE OBRAZU
	//typedef itk::ImageFileReader< ImageType > ReaderType;
	//ReaderType::Pointer reader = ReaderType::New();
	//reader->SetFileName(inputFileName);
	//try
	//{
	//	reader->Update();
	//	std::cout << "Read Image";
	//}
	//catch (itk::ExceptionObject & excep)
	//{
	//	std::cerr << "Exception caught !" << std::endl;
	//	std::cerr << excep << std::endl;
	//	return EXIT_FAILURE;
	//}
	//ImageType::Pointer localImage = reader->GetOutput();

	////TRANSFORMATA HOUGHA
	//std::cout << "Computing Hough Transform" << std::endl;

	//typedef itk::HoughTransform2DCirclesImageFilter<PixelType,
	//	AccumulatorPixelType> HoughTransformFilterType;
	//HoughTransformFilterType::Pointer houghFilter
	//	= HoughTransformFilterType::New();

	//houghFilter->SetInput(reader->GetOutput());
	//houghFilter->SetNumberOfCircles(numberOfCircles);
	//houghFilter->SetMinimumRadius(radiusMin);
	//houghFilter->SetMaximumRadius(radiusMax);
	//houghFilter->SetSweepAngle(sweepAngle);
	//houghFilter->SetSigmaGradient(sigmaGradient);
	//houghFilter->SetVariance(varianceOfAccumulatorBlurring);	
	//houghFilter->SetDiscRadiusRatio(radiusOfTheDiskToRemoveFromTheAccumulator);
	//houghFilter->Update();
	//AccumulatorImageType::Pointer localAccumulator = houghFilter->GetOutput();

	//HoughTransformFilterType::CirclesListType circles;
	//circles = houghFilter->GetCircles();
	//std::cout << "Found " << circles.size() << " circle(s)." << std::endl;
	//std::cout << "- - - - - - - - - - - - - - ";
	//typedef unsigned char OutputPixelType;
	//typedef itk::Image< OutputPixelType, Dimension > OutputImageType;

	//OutputImageType::Pointer localOutputImage = localImage;
	//OutputImageType::RegionType region;
	//region.SetSize(localImage->GetLargestPossibleRegion().GetSize());
	//region.SetIndex(localImage->GetLargestPossibleRegion().GetIndex());
	//localOutputImage->SetRegions(region);
	//localOutputImage->SetOrigin(localImage->GetOrigin());
	//localOutputImage->SetSpacing(localImage->GetSpacing());
	//localOutputImage->Allocate(true); 

	//typedef HoughTransformFilterType::CirclesListType CirclesListType;
	//CirclesListType::const_iterator itCircles = circles.begin();
	//while (itCircles != circles.end())
	//{
	//	std::cout << "\n_____Circle_____\n";
	//	std::cout << "Center: ";
	//	std::cout << (*itCircles)->GetObjectToParentTransform()->GetOffset()
	//		<< std::endl;
	//	std::cout << "Radius: " << (*itCircles)->GetRadius()[0] << std::endl;
	//	std::cout << "\n________________\n";
	//	for (double angle = 0;
	//		angle <= itk::Math::twopi;
	//		angle += itk::Math::pi / 60.0)
	//	{
	//		typedef HoughTransformFilterType::CircleType::TransformType
	//			TransformType;
	//		typedef TransformType::OutputVectorType
	//			OffsetType;
	//		const OffsetType offset =
	//			(*itCircles)->GetObjectToParentTransform()->GetOffset();
	//		localIndex[0] =
	//			itk::Math::Round<long int>(offset[0]
	//				+ (*itCircles)->GetRadius()[0] * std::cos(angle));
	//		localIndex[1] =
	//			itk::Math::Round<long int>(offset[1]
	//				+ (*itCircles)->GetRadius()[0] * std::sin(angle));
	//		OutputImageType::RegionType outputRegion =
	//			localOutputImage->GetLargestPossibleRegion();
	//		if (outputRegion.IsInside(localIndex))
	//		{
	//			localOutputImage->SetPixel(localIndex, 255);
	//		}
	//	}
	//	itCircles++;
	//}
	////ZAPISYWANIE OBRAZU
	//typedef itk::ImageFileWriter< ImageType > WriterType;
	//WriterType::Pointer writer = WriterType::New();
	//writer->SetFileName(outputFileName);
	//writer->SetInput(localOutputImage);
	////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////3D//////////////////////////////////////////
	//ODCZYTANIE SERII
	typedef itk::ImageSeriesReader<ImageType3D> ReaderType3D;
	ReaderType3D::Pointer reader = ReaderType3D::New();

	itk::GDCMSeriesFileNames::Pointer namesGenerator;
	namesGenerator = itk::GDCMSeriesFileNames::New();
	namesGenerator->SetDirectory(inputSeriesPath);
	itk::SerieUIDContainer series = namesGenerator->GetSeriesUIDs();

	reader->SetFileNames(namesGenerator->GetFileNames(series[0]));
	reader->Update();
	ImageType3D::Pointer image3D = reader->GetOutput();

	//ZAPISYWANIE SERII
	typedef itk::ImageSeriesWriter<ImageType3D, ImageType> SeriesWriterType;
	SeriesWriterType::Pointer writer = SeriesWriterType::New();

	itk::NumericSeriesFileNames::Pointer namesSeriesGenerator;
	namesSeriesGenerator = itk::NumericSeriesFileNames::New();
	namesSeriesGenerator->SetSeriesFormat(outputSeriesNames);
	namesSeriesGenerator->SetStartIndex(1);
	namesSeriesGenerator->SetEndIndex(
		image3D->GetLargestPossibleRegion().GetSize()[2]);

	writer->SetFileNames(namesSeriesGenerator->GetFileNames());
	writer->SetInput(image3D);
	///////////////////////////////////////////////////////////////////////////////////////

	try
	{
		writer->Update();
	}
	catch (itk::ExceptionObject & excep)
	{
		std::cerr << "Exception caught !" << std::endl;
		std::cerr << excep << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}