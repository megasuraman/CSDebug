// Copyright 2022 SensyuGames.

#pragma once

#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"
#include "PropertyHandle.h"

class FDataTableRowSelectorCustomization : public IPropertyTypeCustomization
{
public:
	FDataTableRowSelectorCustomization();

	static TSharedRef<IPropertyTypeCustomization> MakeInstance() {
		return MakeShareable(new FDataTableRowSelectorCustomization);
	}

	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle,
		class FDetailWidgetRow& HeaderRow,
		IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle,
		class IDetailChildrenBuilder& StructBuilder,
		IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;


	using TestSelectTypePtr = TSharedPtr<FString>;
	void OnSelectionChanged(TestSelectTypePtr Type, ESelectInfo::Type SelectionType);
	TSharedRef<SWidget> OnGenerateWidget(TestSelectTypePtr Type);
	FText GetSelectedTypeText() const;
	int32 GetSelectIndex() const;

private:
	TSharedPtr<IPropertyHandle> mRowNameHandle;
	TSharedPtr<IPropertyHandle> mDataTablePathHandle;

	TArray< TestSelectTypePtr > OptionTypes;
	TSharedPtr< SComboBox< TestSelectTypePtr > > TypeComboBox;
};