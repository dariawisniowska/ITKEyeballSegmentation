
#include<iostream> // std::cout, std::cin
#include<string> // std::string
#include <windows.h>
#include <stdio.h>

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

#include "itkMeanImageFilter.h"
#include "itkGradientAnisotropicDiffusionImageFilter.h"
#include "itkMinMaxCurvatureFlowImageFilter.h"
#include "itkBinaryErodeImageFilter.h"
#include "itkBinaryDilateImageFilter.h"
#include "itkBinaryBallStructuringElement.h"
#include "itkLaplacianImageFilter.h"

#include "itkSliceBySliceImageFilter.h"

std::string path = "Images/Input/Gsp9_Gsp9/Head_Routine - 20091228/";


/*Pixel Types*/
typedef float PixelType;
typedef float AccumulatorPixelType;
typedef float RadiusPixelType;
typedef float PixelType3D;
typedef float OutputPixelType;

/*Dimensions*/
const unsigned int Dimension = 2;
const int Dimension3D = 3;

/*Image Types*/
typedef itk::Image <PixelType, Dimension > ImageType;
typedef itk::Image <PixelType3D, Dimension3D> ImageType3D;
typedef itk::Image <PixelType3D, Dimension3D> OutImageType3D;
typedef itk::Image <AccumulatorPixelType, Dimension > AccumulatorImageType;
typedef itk::Image <OutputPixelType, Dimension > OutputImageType;

/*Hough Transform Consts*/
constexpr int numberOfCircles = 5;
constexpr float radiusMin = 2.1;
constexpr float radiusMax = 22.4;
constexpr float radiusAvg = 16.2;
constexpr float sweepAngle = 0;
constexpr float sigmaGradient = 1.2;
constexpr float varianceOfAccumulatorBlurring = 10;
constexpr float radiusOfTheDiskToRemoveFromTheAccumulator = 1.1;
constexpr float threshold = 2.3;
ImageType::IndexType localIndex;

/*Defs*/
typedef itk::HoughTransform2DCirclesImageFilter<PixelType, AccumulatorPixelType> HoughTransformFilterType;

HoughTransformFilterType::CirclesListType HoughTransform(ImageType::Pointer image) {

	HoughTransformFilterType::CirclesListType circles;

	typedef itk::CastImageFilter< ImageType, AccumulatorImageType >
		CastingFilterType;
	CastingFilterType::Pointer caster = CastingFilterType::New();
	//std::cout << "Applying gradient magnitude filter" << std::endl;
	typedef itk::GradientMagnitudeImageFilter<AccumulatorImageType,
		AccumulatorImageType > GradientFilterType;
	GradientFilterType::Pointer gradFilter = GradientFilterType::New();
	caster->SetInput(image);
	gradFilter->SetInput(caster->GetOutput());
	gradFilter->Update();

	//TRANSFORMATA HOUGHA
	//std::cout << "Computing Hough Transform" << std::endl;

	HoughTransformFilterType::Pointer houghFilter
		= HoughTransformFilterType::New();

	houghFilter->SetInput(gradFilter->GetOutput());
	houghFilter->SetNumberOfCircles(numberOfCircles);
	houghFilter->SetMinimumRadius(radiusMin);
	houghFilter->SetRadius(radiusAvg);
	houghFilter->SetMaximumRadius(radiusMax);
	houghFilter->SetSweepAngle(sweepAngle);
	houghFilter->SetSigmaGradient(sigmaGradient);
	houghFilter->SetVariance(varianceOfAccumulatorBlurring);
	houghFilter->SetDiscRadiusRatio(radiusOfTheDiskToRemoveFromTheAccumulator);
	houghFilter->SetThreshold(threshold);
	houghFilter->Update();
	AccumulatorImageType::Pointer localAccumulator = houghFilter->GetOutput();

	circles = houghFilter->GetCircles();
	//std::cout << "Found " << circles.size() << " circle(s)." << std::endl;
	//std::cout << "- - - - - - - - - - - - - - \n";

	return circles;
}

int main()
{
	std::string directory = "C:\\POMwJO\\ITK-projekt\\Images\\Input\\mozg md\\Head_Neck_Standard - 1\\t1_vibe_fs_hi_res_20";
	itk::GDCMSeriesFileNames::Pointer namesGenerator;
	namesGenerator = itk::GDCMSeriesFileNames::New();
	namesGenerator->SetDirectory(directory);
	itk::SerieUIDContainer series = namesGenerator->GetSeriesUIDs();
	for (size_t i = 0; i < series.size(); i++)
	{
		itk::FilenamesContainer filenames = namesGenerator->GetFileNames(series[i]);
		itk::NumericSeriesFileNames::Pointer namesSeriesGenerator;
		namesSeriesGenerator = itk::NumericSeriesFileNames::New();
		namesSeriesGenerator->SetSeriesFormat("C:\\POMwJO\\ITK-projekt\\Images\\Output\\IMG%05d.dcm");
		namesSeriesGenerator->SetStartIndex(1);
		namesSeriesGenerator->SetEndIndex(filenames.size());
		itk::FilenamesContainer outputFilenames = namesSeriesGenerator->GetFileNames();
		for (size_t k = 0; k < filenames.size(); k++)
		{
			
			typedef itk::ImageFileReader< ImageType > ReaderType;
			ReaderType::Pointer reader = ReaderType::New();
			reader->SetFileName(filenames[k]);
			try
			{
				reader->Update();
				std::cout << "- - - - - - - - - - - - - - \n";
				std::cout << "Read Image: " << filenames[k] << std::endl;
				std::cout << "- - - - - - - - - - - - - - \n";
			}
			catch (itk::ExceptionObject & excep)
			{
				std::cerr << "Exception caught !" << std::endl;
				std::cerr << excep << std::endl;
				return EXIT_FAILURE;
			}
			ImageType::Pointer localImage = reader->GetOutput();

			OutputImageType::Pointer localOutputImage = localImage;
			OutputImageType::RegionType region;
			region.SetSize(localImage->GetLargestPossibleRegion().GetSize());
			region.SetIndex(localImage->GetLargestPossibleRegion().GetIndex());
			localOutputImage->SetRegions(region);
			localOutputImage->SetOrigin(localImage->GetOrigin());
			localOutputImage->SetSpacing(localImage->GetSpacing());
			localOutputImage->Allocate(true);

			HoughTransformFilterType::CirclesListType circles = HoughTransform(localImage);

			typedef HoughTransformFilterType::CirclesListType CirclesListType;
			CirclesListType::const_iterator itCircles = circles.begin();
			while (itCircles != circles.end())
			{
				std::cout << "Center: " << (*itCircles)->GetObjectToParentTransform()->GetOffset() << std::endl;
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
						localOutputImage->SetPixel(localIndex, 1024);
					}
				}
				itCircles++;
			}
			//ZAPISYWANIE OBRAZU
			typedef itk::ImageFileWriter< ImageType > WriterType;
			WriterType::Pointer writer = WriterType::New();
			writer->SetFileName(outputFilenames[k]);
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
		}
	}
	
	

	return EXIT_SUCCESS;
}

