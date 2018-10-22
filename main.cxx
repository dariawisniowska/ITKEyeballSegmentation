
#include<iostream> // std::cout, std::cin
#include<string> // std::string
#include<itkImageFileReader.h>
#include<itkImageFileWriter.h>
#include<itkHoughTransform2DCirclesImageFilter.h>
#include<itkThresholdImageFilter.h>
#include<itkImageRegionIterator.h>
#include<itkMinimumMaximumImageCalculator.h>
#include<itkGradientMagnitudeImageFilter.h>
#include<itkDiscreteGaussianImageFilter.h>
#include<itkCastImageFilter.h>
#include "itkEllipseSpatialObject.h"
#include<itkMath.h>

// g³ówna funkcja programu
int main()
{
	constexpr int numberOfCircles = 2;
	constexpr int radiusMin = 10;
	constexpr int radiusMax = 50;
	constexpr int sweepAngle = 0;
	constexpr int sigmaGradient = 1;
	constexpr int varianceOfAccumulatorBlurring = 5;
	constexpr int radiusOfTheDiskToRemoveFromTheAccumulator = 10;
	char* inputFileName = "Images/Gsp12_Gsp12/Head_Routine - 20090514/T1_SE_TRA_PAT2_FIL_4/IM-0002-0004.dcm";
	char* outputFileName = "IM-0002-0004-C.dcm";

	typedef unsigned char PixelType;
	typedef float AccumulatorPixelType;
	typedef float RadiusPixelType;
	const unsigned int Dimension = 2;
	typedef itk::Image< PixelType, Dimension > ImageType;
	ImageType::IndexType localIndex;
	typedef itk::Image< AccumulatorPixelType, Dimension > AccumulatorImageType;

	//ODCZYTYWANIE OBRAZU
	typedef itk::ImageFileReader< ImageType > ReaderType;
	ReaderType::Pointer reader = ReaderType::New();
	reader->SetFileName(inputFileName);
	try
	{
		reader->Update();
	}
	catch (itk::ExceptionObject & excep)
	{
		std::cerr << "Exception caught !" << std::endl;
		std::cerr << excep << std::endl;
		return EXIT_FAILURE;
	}
	ImageType::Pointer localImage = reader->GetOutput();

	//TRANSFORMATA HOUGHA
	std::cout << "Computing Hough Map" << std::endl;

	typedef itk::HoughTransform2DCirclesImageFilter<PixelType,
		AccumulatorPixelType> HoughTransformFilterType;
	HoughTransformFilterType::Pointer houghFilter
		= HoughTransformFilterType::New();

	houghFilter->SetInput(reader->GetOutput());
	houghFilter->SetNumberOfCircles(numberOfCircles);
	houghFilter->SetMinimumRadius(radiusMin);
	houghFilter->SetMaximumRadius(radiusMax);
	houghFilter->SetSweepAngle(sweepAngle);
	houghFilter->SetSigmaGradient(sigmaGradient);
	houghFilter->SetVariance(varianceOfAccumulatorBlurring);	
	houghFilter->SetDiscRadiusRatio(radiusOfTheDiskToRemoveFromTheAccumulator);
	houghFilter->Update();
	AccumulatorImageType::Pointer localAccumulator = houghFilter->GetOutput();

	HoughTransformFilterType::CirclesListType circles;
	circles = houghFilter->GetCircles();
	std::cout << "Found " << circles.size() << " circle(s)." << std::endl;

	typedef unsigned char OutputPixelType;
	typedef itk::Image< OutputPixelType, Dimension > OutputImageType;

	OutputImageType::Pointer localOutputImage = OutputImageType::New();
	OutputImageType::RegionType region;
	region.SetSize(localImage->GetLargestPossibleRegion().GetSize());
	region.SetIndex(localImage->GetLargestPossibleRegion().GetIndex());
	localOutputImage->SetRegions(region);
	localOutputImage->SetOrigin(localImage->GetOrigin());
	localOutputImage->SetSpacing(localImage->GetSpacing());
	localOutputImage->Allocate(true); 

	typedef HoughTransformFilterType::CirclesListType CirclesListType;
	CirclesListType::const_iterator itCircles = circles.begin();
	while (itCircles != circles.end())
	{
		std::cout << "Center: ";
		std::cout << (*itCircles)->GetObjectToParentTransform()->GetOffset()
			<< std::endl;
		std::cout << "Radius: " << (*itCircles)->GetRadius()[0] << std::endl;
		for (double angle = 0;
			angle <= itk::Math::twopi;
			angle += itk::Math::pi / 60.0)
		{
			typedef HoughTransformFilterType::CircleType::TransformType
				TransformType;
			typedef TransformType::OutputVectorType
				OffsetType;
			const OffsetType offset =
				(*itCircles)->GetObjectToParentTransform()->GetOffset();
			localIndex[0] =
				itk::Math::Round<long int>(offset[0]
					+ (*itCircles)->GetRadius()[0] * std::cos(angle));
			localIndex[1] =
				itk::Math::Round<long int>(offset[1]
					+ (*itCircles)->GetRadius()[0] * std::sin(angle));
			OutputImageType::RegionType outputRegion =
				localOutputImage->GetLargestPossibleRegion();
			if (outputRegion.IsInside(localIndex))
			{
				localOutputImage->SetPixel(localIndex, 255);
			}
		}
		itCircles++;
	}
	//ZAPISYWANIE OBRAZU
	typedef itk::ImageFileWriter< ImageType > WriterType;
	WriterType::Pointer writer = WriterType::New();
	writer->SetFileName(outputFileName);
	writer->SetInput(localOutputImage);
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