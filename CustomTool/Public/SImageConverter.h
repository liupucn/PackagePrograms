#pragma once
#include "Widgets/SCompoundWidget.h"

class CUSTOMTOOL_API SImageConverter : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SImageConverter)
		:_bAutoFillSize(false)
	{}
		SLATE_ARGUMENT(bool, bAutoFillSize)
	SLATE_END_ARGS() 

public:
	SImageConverter();
	~SImageConverter();

	void Construct(const FArguments& InArgs);

private:

	FText GetPortText() const;

	void OnPortTextCommited(const FText& InText, ETextCommit::Type InCommitType);

	FReply OnClicked();

	FText GetDebugMsg() const;

	bool ConvertPNGToTxt(const FString& InFilePath, const FString& OutFilePath);

private:
	FString DefaultPath;
	FString DebugMsg;
};

#define DEBUG_MSG(Msg)\
DebugMsg+=Msg;\
DebugMsg+="\n";\
