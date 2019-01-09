
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
#include "itkEllipseSpatialObject.h"

/*Pixel Types*/
typedef float PixelType;
typedef float AccumulatorPixelType;
typedef float OutputPixelType;

/*Dimensions*/
const unsigned int Dimension = 2;

/*Image Types*/
typedef itk::Image <PixelType, Dimension > ImageType;
typedef itk::Image <AccumulatorPixelType, Dimension > AccumulatorImageType;
typedef itk::Image <OutputPixelType, Dimension > OutputImageType;

/*Hough Transform Consts*/
float radiusMax_temp = 35/2;
float radiusAvg_temp = 24/2;
float spacing = 1;
constexpr int numberOfCircles = 5;
constexpr float radiusMin = 2.1; 
float radiusMax = 22.1; //potem nadpisywane po przeliczeniu _temp wed³ug spacing
float radiusAvg = 16.2; 
constexpr float sweepAngle = 0;
constexpr float sigmaGradient = 1.2;
constexpr float varianceOfAccumulatorBlurring = 10;
constexpr float radiusOfTheDiskToRemoveFromTheAccumulator = 1.1;
constexpr float threshold = 6.3;
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
	char plaszczyna = 'S';
	std::string directory;
	switch (plaszczyna)
	{
	case 'C':
		directory = "C:\\POMwJO\\ITK-projekt\\Images\\Input\\Gsp8_Gsp8\\Head_Routine - 20090212\\T1_FL3D_COR_CM_24";
		break;
	case 'P':
		directory = "C:\\POMwJO\\ITK-projekt\\Images\\Input\\Gsp13_Gsp13\\Head_Routine - 20090915\\T1_SE_TRA_PAT2_4";
		break;
	case 'S':
		directory = "C:\\POMwJO\\ITK-projekt\\Images\\Input\\mozg md\\Head_Neck_Standard - 1\\t1_tse_sag_2";
		break;
	}
	itk::GDCMSeriesFileNames::Pointer namesGenerator;
	namesGenerator = itk::GDCMSeriesFileNames::New();
	namesGenerator->SetDirectory(directory);

	itk::SerieUIDContainer series = namesGenerator->GetSeriesUIDs();
	std::cout << "itk = [  ";
	for (size_t i = 0; i < series.size(); i++)
	{
		itk::FilenamesContainer filenames = namesGenerator->GetFileNames(series[i]);
		itk::NumericSeriesFileNames::Pointer namesSeriesGenerator;
		namesSeriesGenerator = itk::NumericSeriesFileNames::New();
		switch (plaszczyna)
		{
		case 'C':
			namesSeriesGenerator->SetSeriesFormat("C:\\POMwJO\\ITK-projekt\\Images\\Output\\Czolowa\\IMG%03d.dcm");
			break;
		case 'P':
			namesSeriesGenerator->SetSeriesFormat("C:\\POMwJO\\ITK-projekt\\Images\\Output\\Poprzeczna\\IMG%03d.dcm");
			break;
		case 'S':
			namesSeriesGenerator->SetSeriesFormat("C:\\POMwJO\\ITK-projekt\\Images\\Output\\Strzalkowa\\IMG%03d.dcm");
			break;
		}
		namesSeriesGenerator->SetStartIndex(1);
		namesSeriesGenerator->SetEndIndex(filenames.size());
		itk::FilenamesContainer outputFilenames = namesSeriesGenerator->GetFileNames();

		std::vector<HoughTransformFilterType::CirclesListType> circles(filenames.size());
		std::vector<HoughTransformFilterType::CirclesListType> savedCircles(filenames.size());
		std::vector<HoughTransformFilterType::CirclesListType> finalCircles(filenames.size());

		ImageType::Pointer localImage;
		for (size_t k = 0; k < filenames.size(); k++)
		{

			typedef itk::ImageFileReader< ImageType > ReaderType;
			ReaderType::Pointer reader = ReaderType::New();
			reader->SetFileName(filenames[k]);
			try
			{
				reader->Update();
				//std::cout << "- - - - - - - - - - - - - - \n";
				//std::cout << "Read Image: " << filenames[k] << std::endl;
				//std::cout << "- - - - - - - - - - - - - - \n";
			}
			catch (itk::ExceptionObject & excep)
			{
				std::cerr << "Exception caught !" << std::endl;
				std::cerr << excep << std::endl;
				return EXIT_FAILURE;
			}
			localImage = reader->GetOutput();
			spacing = localImage->GetSpacing()[0];
			radiusMax = radiusMax_temp / spacing ;
			radiusAvg = radiusAvg_temp / spacing;
			circles[k] = HoughTransform(localImage);
			//std::cout << "		Found " <<circles[k].size()<<" circles. "<< std::endl;
		}
	
		//std::cout << "- - - - - - - - - - - - - - \n";
		//std::cout << "Validation" << std::endl;
		//std::cout << "- - - - - - - - - - - - - - \n";
		typedef HoughTransformFilterType::CirclesListType CirclesListType;
		savedCircles = circles;
		//finalCircles = savedCircles;
		for (size_t j = 0; j < circles.size() ; j++)
		{
			CirclesListType::const_iterator itCircles = circles[j].begin();
			while (itCircles != circles[j].end())
			{
				bool warunek;
				switch (plaszczyna)
				{
				case 'C':
					warunek = ((*itCircles)->GetObjectToParentTransform()->GetOffset()[1] < localImage->GetLargestPossibleRegion().GetSize()[1] / 2) && ((*itCircles)->GetObjectToParentTransform()->GetOffset()[1] > localImage->GetLargestPossibleRegion().GetSize()[1] / 3);
					break;
				case 'P':
					warunek = (*itCircles)->GetObjectToParentTransform()->GetOffset()[1] < localImage->GetLargestPossibleRegion().GetSize()[1] / 2;
					break;
				case 'S':
					warunek = (*itCircles)->GetObjectToParentTransform()->GetOffset()[0] < localImage->GetLargestPossibleRegion().GetSize()[0] / 2;
					break;
				}
				if(warunek)
				{
					if (j >= 2 && j < circles.size() - 2)
					{
						//pêtla po preprevious
						bool preprevious = false;
						size_t prog = 10;
						CirclesListType::const_iterator itCirclesPrePrevious = circles[j - 2].begin();
						while (itCirclesPrePrevious != circles[j - 2].end())
						{
							if ((((*itCircles)->GetObjectToParentTransform()->GetOffset()[0] >= (*itCirclesPrePrevious)->GetObjectToParentTransform()->GetOffset()[0] - prog && (*itCircles)->GetObjectToParentTransform()->GetOffset()[0] <= (*itCirclesPrePrevious)->GetObjectToParentTransform()->GetOffset()[0] + prog)
								&& ((*itCircles)->GetObjectToParentTransform()->GetOffset()[1] >= (*itCirclesPrePrevious)->GetObjectToParentTransform()->GetOffset()[1] - prog && (*itCircles)->GetObjectToParentTransform()->GetOffset()[1] <= (*itCirclesPrePrevious)->GetObjectToParentTransform()->GetOffset()[1] + prog)))
							{

								preprevious = true;

							}
							itCirclesPrePrevious++;
						}
						//pêtla po previous
						bool previous = false;
						CirclesListType::const_iterator itCirclesPrevious = circles[j - 1].begin();
						while (itCirclesPrevious != circles[j - 1].end())
						{
							if ((((*itCircles)->GetObjectToParentTransform()->GetOffset()[0] >= (*itCirclesPrevious)->GetObjectToParentTransform()->GetOffset()[0] - prog && (*itCircles)->GetObjectToParentTransform()->GetOffset()[0] <= (*itCirclesPrevious)->GetObjectToParentTransform()->GetOffset()[0] + prog)
								&& ((*itCircles)->GetObjectToParentTransform()->GetOffset()[1] >= (*itCirclesPrevious)->GetObjectToParentTransform()->GetOffset()[1] - prog && (*itCircles)->GetObjectToParentTransform()->GetOffset()[1] <= (*itCirclesPrevious)->GetObjectToParentTransform()->GetOffset()[1] + prog)))
							{

								previous = true;

							}
							itCirclesPrevious++;
						}
						//pêtle po next
						bool next = false;
						CirclesListType::const_iterator itCirclesNext = circles[j + 1].begin();
						while (itCirclesNext != circles[j + 1].end())
						{
							if ((((*itCircles)->GetObjectToParentTransform()->GetOffset()[0] >= (*itCirclesNext)->GetObjectToParentTransform()->GetOffset()[0] - prog && (*itCircles)->GetObjectToParentTransform()->GetOffset()[0] <= (*itCirclesNext)->GetObjectToParentTransform()->GetOffset()[0] + prog)
								&& ((*itCircles)->GetObjectToParentTransform()->GetOffset()[1] >= (*itCirclesNext)->GetObjectToParentTransform()->GetOffset()[1] - prog && (*itCircles)->GetObjectToParentTransform()->GetOffset()[1] <= (*itCirclesNext)->GetObjectToParentTransform()->GetOffset()[1] + prog)))
							{

								next = true;

							}
							itCirclesNext++;
						}
						//pêtle po next
						bool postnext = false;
						CirclesListType::const_iterator itCirclesPostNext = circles[j + 2].begin();
						while (itCirclesPostNext != circles[j + 2].end())
						{
							if ((((*itCircles)->GetObjectToParentTransform()->GetOffset()[0] >= (*itCirclesPostNext)->GetObjectToParentTransform()->GetOffset()[0] - prog && (*itCircles)->GetObjectToParentTransform()->GetOffset()[0] <= (*itCirclesPostNext)->GetObjectToParentTransform()->GetOffset()[0] + prog)
								&& ((*itCircles)->GetObjectToParentTransform()->GetOffset()[1] >= (*itCirclesPostNext)->GetObjectToParentTransform()->GetOffset()[1] - prog && (*itCircles)->GetObjectToParentTransform()->GetOffset()[1] <= (*itCirclesPostNext)->GetObjectToParentTransform()->GetOffset()[1] + prog)))
							{

								postnext = true;

							}
							itCirclesPostNext++;
						}
						//if to albo to true zostaje 
						if (previous && next)
						{
							if (previous && preprevious)
							{
								//std::cout << "saved" << std::endl;
							}
							else if (next && postnext)
							{
								//std::cout << "saved" << std::endl;
							}
							else
							{
								//std::cout << "popped" << std::endl;
								savedCircles[j].remove((*itCircles));
							}

						}
						else
						{
							//std::cout << "popped" << std::endl;
							savedCircles[j].remove((*itCircles));
						}
					}
					else
						savedCircles[j].remove((*itCircles));
				}
				else
					savedCircles[j].remove((*itCircles));
				
				itCircles++;
			}

			finalCircles[j] = savedCircles[j];
		}


		for (size_t k = 0; k < filenames.size(); k++)
		{

			typedef itk::ImageFileReader< ImageType > ReaderType;
			ReaderType::Pointer reader = ReaderType::New();
			reader->SetFileName(filenames[k]);
			try
			{
				reader->Update();
				/*std::cout << "- - - - - - - - - - - - - - \n";
				std::cout << "Read Image: " << filenames[k] << std::endl;
				std::cout << "- - - - - - - - - - - - - - \n";*/
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
			
			circles[k] = finalCircles[k];
			//std::cout << "		Saved " << circles[k].size() << " circles. " << std::endl;

			typedef HoughTransformFilterType::CirclesListType CirclesListType;
			CirclesListType::const_iterator itCircles = circles[k].begin();
			while (itCircles != circles[k].end())
			{
				//std::cout << "Center: " << (*itCircles)->GetObjectToParentTransform()->GetOffset() << std::endl;
				//std::cout << "Radius: " << (*itCircles)->GetRadius()[0] << std::endl;
				std::cout << (*itCircles)->GetObjectToParentTransform()->GetOffset()[0] <<", "<<(*itCircles)->GetObjectToParentTransform()->GetOffset()[1]<<", " << (*itCircles)->GetRadius()[0];
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

				if((itCircles != circles[k].end()))
					std::cout << " , " ;
			}

			if (circles[k].size() == 0)
				std::cout << " 0,0,0,0,0,0";

			if(circles[k].size()==1)
				std::cout << " ,0,0,0";
			
			//ZAPISYWANIE OBRAZU
			typedef itk::ImageFileWriter< ImageType > WriterType;
			WriterType::Pointer writer = WriterType::New();
			writer->SetFileName(outputFilenames[k]);
			writer->SetInput(localOutputImage);
			try
			{
				writer->Update();
				//std::cout << "		Overwrited image" << std::endl;
			}
			catch (itk::ExceptionObject & excep)
			{
				std::cerr << "Exception caught !" << std::endl;
				std::cerr << excep << std::endl;
				return EXIT_FAILURE;
			}
			std::cout << " ;" << std::endl;
		}
		std::cout << " ];" << std::endl;
	}
	

	return EXIT_SUCCESS;
}

