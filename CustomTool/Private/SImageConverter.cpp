#include "SImageConverter.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Misc/FileHelper.h"
#include "Async/Async.h"

SImageConverter::SImageConverter()
{
	DefaultPath = TEXT("D:/Heightmap");
}

SImageConverter::~SImageConverter()
{
}

void SImageConverter::Construct(const FArguments& InArgs)
{
	static const FTextBlockStyle MetadataTextStyle = FTextBlockStyle(FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText"))
		.SetFontSize(10);

	TSharedPtr<SWidget> MainShowWidgetPtr = nullptr;
	
	MainShowWidgetPtr = SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.VAlign(VAlign_Center)
		.Padding(10, 10, 0, 0)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.FillWidth(1)
			[
				SNew(SEditableTextBox)
				.Font(FCoreStyle::Get().GetFontStyle("SmallFont"))
				.Text(this, &SImageConverter::GetPortText)
				.OnTextCommitted(this, &SImageConverter::OnPortTextCommited)
			]
			+SHorizontalBox::Slot()
			.FillWidth(1)
			.AutoWidth()
			[
				SNew(SButton)
				.ContentPadding(FMargin(10, 2))
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Center)
				.Text(FText::FromString(TEXT("开始处理")))
				.OnClicked(this, &SImageConverter::OnClicked)
			]
		]
		+ SVerticalBox::Slot()
		.VAlign(VAlign_Top)
		.Padding(10, 10, 0, 0)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(1)
			[
				SNew(SBox)
				.HeightOverride(500)
				[
					SNew(SMultiLineEditableTextBox)
					.Text(this, &SImageConverter::GetDebugMsg)
					.IsReadOnly(true)
				]
			]
		];

	ChildSlot
	[
		MainShowWidgetPtr.ToSharedRef()
	];
}

FText SImageConverter::GetPortText() const
{
	return FText::FromString(DefaultPath);
}

void SImageConverter::OnPortTextCommited(const FText& InText, ETextCommit::Type InCommitType)
{
	DefaultPath = InText.ToString();
}

FReply SImageConverter::OnClicked()
{
	if(!FPaths::DirectoryExists(DefaultPath))
	{
		DEBUG_MSG(TEXT("Error: 路径不存在"));
	}

	TArray<FString> FoundFiles;
	IFileManager::Get().FindFiles(FoundFiles, *DefaultPath, TEXT("*.png"));

	for (auto& Elem : FoundFiles)
	{
		FString SrcFilePath = FPaths::Combine(DefaultPath, Elem);
		FString OutFilePath = FPaths::Combine(DefaultPath, FPaths::GetBaseFilename(Elem)+ TEXT(".txt"));

		DEBUG_MSG(TEXT("正在处理: ") + SrcFilePath);

		AsyncTask(ENamedThreads::AnyThread, [=]()
		{
			if (ConvertPNGToTxt(SrcFilePath, OutFilePath))
			{
				DEBUG_MSG(TEXT("成功: ") + OutFilePath);
			}
			else
			{
				DEBUG_MSG(TEXT("失败: ") + OutFilePath);
			}
		});
	}


	return FReply::Handled();
}

FText SImageConverter::GetDebugMsg() const
{
	return FText::FromString(DebugMsg);
}

bool SImageConverter::ConvertPNGToTxt(const FString& InFilePath, const FString& OutFilePath)
{
	TArray<uint8> Data;
	if(!FFileHelper::LoadFileToArray(Data, *InFilePath))
		return false;
		
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	IImageWrapperPtr ImageWrapperPtr = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

	if (ImageWrapperPtr.IsValid() && ImageWrapperPtr->SetCompressed(Data.GetData(), Data.Num()))
	{
		TArray<uint8> RGBAData;
		FString Output;
		if (ImageWrapperPtr->GetRaw(ERGBFormat::RGBA, 8, RGBAData))
		{
			FString ImageInfo = FString::Printf(
				TEXT("ImageInfo: width=%d, height=%d"), 
				ImageWrapperPtr->GetWidth(), 
				ImageWrapperPtr->GetHeight());

			DEBUG_MSG(ImageInfo);

			for (int i= 0; i< RGBAData.Num(); i+=4)
			{
				FString NewRGBAStr = FString::Printf(TEXT("%d"), RGBAData[i]);

				Output += NewRGBAStr;

				if((i/4 + 1) % ImageWrapperPtr->GetWidth() == 0 && i!=0)
					Output+="\n";
				else
					Output += "\t";
			}
			
			return FFileHelper::SaveStringToFile(Output, *OutFilePath);
		}
	}

	return false;
}