/* komentarze
 wielolinijkowe */

 // do³aczanie plików nag³ówkowych
#include<iostream> // std::cout, std::cin
#include<string> // std::string
#include<itkImageFileReader.h>
#include<itkImageFileWriter.h>

// g³ówna funkcja programu
int main()
{
	// instrukcje programu
	typedef char PixelType;
	typedef itk::Image<PixelType, 2> ImageType;
	typedef itk::ImageFileReader<ImageType> ReaderType;

	//itk::ImageFileReader<itk::Image<char, 2>> reader = new itk::ImageFileReader<itk::Image<char, 2>>();

	ReaderType::Pointer reader = ReaderType::New();
	reader->SetFileName("Images/Gsp12_Gsp12/Head_Routine - 20090514/T1_SE_TRA_PAT2_FIL_4/IM-0002-0004.dcm");


	try
	{
		reader->Update(); // !!!
	}
	catch (itk::ExceptionObject e)
	{
		std::cout << e;
	}
	ImageType::Pointer image = reader->GetOutput();
	std::cout << image;

	//typedef itk::ImageFileWriter<ImageType> WriterType;

	//WriterType::Pointer writer = WriterType::New();
	//writer->SetFileName("nowy-plik.dcm");
	//writer->SetInput(image);
	//writer->Update();


	std::cin.get(); // oczekiwanie na [Enter]
	return EXIT_SUCCESS; // albo EXIT_FAILURE
}