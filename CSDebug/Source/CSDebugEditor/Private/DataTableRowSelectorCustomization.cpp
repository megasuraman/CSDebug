// Copyright 2022 SensyuGames.


#include "DataTableRowSelectorCustomization.h"
#include "PropertyEditing.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "DataTableRowSelector.h"

#define LOCTEXT_NAMESPACE "FCSDebugEditor"

FDataTableRowSelectorCustomization::FDataTableRowSelectorCustomization()
{
}

void FDataTableRowSelectorCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle,
	class FDetailWidgetRow& HeaderRow,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	mRowNameHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FDataTableRowSelector, mRowName));

 	//FString aDataTablePath("/Game/DT_Test.DT_Test");
 	//if (UDataTable* aDataTable = LoadObject<UDataTable>(nullptr, (*aDataTablePath), nullptr, LOAD_None, nullptr))
 	//{
 	//	for (const FName& RowName : aDataTable->GetRowNames())
 	//	{
 	//		OptionTypes.Add(MakeShareable(new FString(RowName.ToString())));
 	//	}
 	//}
	mDataTablePathHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FDataTableRowSelector, mDataTablePath));
 	if (!mDataTablePathHandle.IsValid())
 	{
 		return;
 	}
 	FString DataTablePath;
 	if (mDataTablePathHandle->GetValue(DataTablePath) != FPropertyAccess::Success)
 	{
 		return;
 	}
 
 	if (UDataTable* aDataTable = LoadObject<UDataTable>(nullptr, (*DataTablePath), nullptr, LOAD_None, nullptr))
 	{
 		for (const FName& RowName : aDataTable->GetRowNames())
 		{
 			OptionTypes.Add(MakeShareable(new FString(RowName.ToString())));
 		}
 	}
}

void FDataTableRowSelectorCustomization::CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle,
	class IDetailChildrenBuilder& StructBuilder,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	if (OptionTypes.Num() <= 0)
	{
		return;
	}

	FString DispPropertyName(TEXT("DataTableRowName"));
	if (mDataTablePathHandle.IsValid())
	{
		FString DataTablePath;
		if (mDataTablePathHandle->GetValue(DataTablePath) == FPropertyAccess::Success)
		{
			FString LeftString;
			DataTablePath.Split(TEXT("."), &LeftString, &DispPropertyName);
		}
	}

	StructBuilder.AddCustomRow(LOCTEXT("TypeRow", "TypeRow"))
	.NameContent()
	[
		SNew(STextBlock)
		.Text(FText::Format(LOCTEXT("TypeTitle", "{DispPropertyName}"), FText::FromString(DispPropertyName)))
	]
	.ValueContent()
	[
		SAssignNew(TypeComboBox, SComboBox<TestSelectTypePtr>)
		.OptionsSource(&OptionTypes)
		.InitiallySelectedItem(OptionTypes[GetSelectIndex()])
		.OnSelectionChanged(this, &FDataTableRowSelectorCustomization::OnSelectionChanged)
		.OnGenerateWidget(this, &FDataTableRowSelectorCustomization::OnGenerateWidget)
		[
			SNew(STextBlock)
			.Text(this, &FDataTableRowSelectorCustomization::GetSelectedTypeText)
		]
	];
}


void FDataTableRowSelectorCustomization::OnSelectionChanged(TestSelectTypePtr Type, ESelectInfo::Type SelectionType)
{
	if (mRowNameHandle.IsValid())
	{
		mRowNameHandle->SetValue(*Type);
	}
}

TSharedRef<SWidget> FDataTableRowSelectorCustomization::OnGenerateWidget(TestSelectTypePtr Type)
{
	return SNew(STextBlock).Text(FText::FromString(*Type));
}
FText FDataTableRowSelectorCustomization::GetSelectedTypeText() const
{
	TestSelectTypePtr SelectedType = TypeComboBox->GetSelectedItem();
	return FText::FromString(*SelectedType);
}
int32 FDataTableRowSelectorCustomization::GetSelectIndex() const
{
	if (!mRowNameHandle.IsValid())
	{
		return 0;
	}
	FName RowName;
	if (mRowNameHandle->GetValue(RowName) != FPropertyAccess::Success)
	{
		return 0;
	}

	for (int32 i = 0; i < OptionTypes.Num(); ++i)
	{
		if (*OptionTypes[i] == RowName.ToString())
		{
			return i;
		}
	}
	return 0;
}

#undef LOCTEXT_NAMESPACE