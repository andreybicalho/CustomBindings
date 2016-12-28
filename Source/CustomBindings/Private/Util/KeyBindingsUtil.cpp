// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomBindings.h"
#include "KeyBindingsUtil.h"
#include "Runtime/Engine/Classes/GameFramework/InputSettings.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectGlobals.h"

// publics

UKeyBindingsUtil::UKeyBindingsUtil(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

FInputAction UKeyBindingsUtil::GetInputActionFromKeyEvent(const FKeyEvent& KeyEvent)
{
	FInputAction InputAction;

	InputAction.Key = KeyEvent.GetKey();
	InputAction.KeyAsString = InputAction.Key.GetDisplayName().ToString();

	InputAction.bAlt = KeyEvent.IsAltDown();
	InputAction.bCtrl = KeyEvent.IsControlDown();
	InputAction.bShift = KeyEvent.IsShiftDown();
	InputAction.bCmd = KeyEvent.IsCommandDown();

	return InputAction;
}

FInputAxis UKeyBindingsUtil::GetInputAxisFromKeyEvent(const FKeyEvent& KeyEvent)
{
	FInputAxis InputAxis;

	InputAxis.Key = KeyEvent.GetKey();
	InputAxis.KeyAsString = InputAxis.Key.GetDisplayName().ToString();

	InputAxis.Scale = 1;

	return InputAxis;
}

void UKeyBindingsUtil::GetAllInputActionKeyBindings(TArray<FInputAction>& InputActions)
{	
	InputActions.Empty();

	const UInputSettings* Settings = GetDefault<UInputSettings>();
	if (!Settings) return;

	const TArray<FInputActionKeyMapping>& Actions = Settings->ActionMappings;

	for (const FInputActionKeyMapping& Each : Actions)
	{
		InputActions.Add(FInputAction(Each));
	}
}

void UKeyBindingsUtil::GetAllInputAxisKeyBindings(TArray<FInputAxis>& InputAxis)
{
	InputAxis.Empty();

	const UInputSettings* Settings = GetDefault<UInputSettings>();
	if (!Settings) return;

	const TArray<FInputAxisKeyMapping>& Axis = Settings->AxisMappings;

	for (const FInputAxisKeyMapping& Each : Axis)
	{
		InputAxis.Add(FInputAxis(Each));
	}
}

bool UKeyBindingsUtil::ReBindAxisKey(FInputAxis CurrentBinding, FInputAxis NewBinding)
{
	UInputSettings* Settings = const_cast<UInputSettings*>(GetDefault<UInputSettings>());
	if (!Settings) return false;

	TArray<FInputAxisKeyMapping>& Axis = Settings->AxisMappings;

	bool Found = false;
	for (FInputAxisKeyMapping& Each : Axis)
	{
		//Search by CurrentBinding
		if (Each.AxisName.ToString() == CurrentBinding.AxisName && Each.Key == CurrentBinding.Key) {
			//Update to new!
			UKeyBindingsUtil::UpdateAxisBinding(NewBinding, Each);
			Found = true;
			break;
		}
	}

	if (Found)
	{
		//SAVES TO DISK
		const_cast<UInputSettings*>(Settings)->SaveKeyMappings();

		//REBUILDS INPUT, creates modified config in Saved/Config/Windows/Input.ini
		for (TObjectIterator<UPlayerInput> It; It; ++It)
		{
			It->ForceRebuildingKeyMaps(true);
		}
	}
	return Found;
}

bool UKeyBindingsUtil::ReBindActionKey(FInputAction CurrentBinding, FInputAction NewBinding)
{
	UInputSettings* Settings = const_cast<UInputSettings*>(GetDefault<UInputSettings>());
	if (!Settings) return false;

	TArray<FInputActionKeyMapping>& Actions = Settings->ActionMappings;

	bool Found = false;
	for (FInputActionKeyMapping& Each : Actions)
	{
		//Search by the current binding
		if (Each.ActionName.ToString() == CurrentBinding.ActionName && Each.Key == CurrentBinding.Key) {
			//Update to new!
			UKeyBindingsUtil::UpdateActionBinding(NewBinding, Each);
			Found = true;
			break;
		}
	}

	if (Found)
	{
		//SAVES TO DISK
		const_cast<UInputSettings*>(Settings)->SaveKeyMappings();

		//REBUILDS INPUT, creates modified config in Saved/Config/Windows/Input.ini
		for (TObjectIterator<UPlayerInput> It; It; ++It)
		{
			It->ForceRebuildingKeyMaps(true);
		}
	}
	return Found;
}

// privates

void UKeyBindingsUtil::UpdateAxisBinding(const FInputAxis& SrcInputAxis, FInputAxisKeyMapping& DstInputAxis)
{
	DstInputAxis.Key = SrcInputAxis.Key;
	DstInputAxis.Scale = SrcInputAxis.Scale;
}

void UKeyBindingsUtil::UpdateActionBinding(const FInputAction& SrcInputAction, FInputActionKeyMapping& DstInputAction)
{
	DstInputAction.Key = SrcInputAction.Key;
	DstInputAction.bShift = SrcInputAction.bShift;
	DstInputAction.bCtrl = SrcInputAction.bCtrl;
	DstInputAction.bAlt = SrcInputAction.bAlt;
	DstInputAction.bCmd = SrcInputAction.bCmd;
}


