// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomBindings.h"
#include "KeyBindingUtil.h"
#include "Runtime/Engine/Classes/GameFramework/InputSettings.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectGlobals.h"


UKeyBindingUtil::UKeyBindingUtil(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

/*
* 
*/
void UKeyBindingUtil::UpdateAxisBinding(const FInputAxis& SrcInputAxis, FInputAxisKeyMapping& DstInputAxis)
{
	DstInputAxis.Key = SrcInputAxis.Key;
	DstInputAxis.Scale = SrcInputAxis.Scale;
}

/*
*
*/
void UKeyBindingUtil::UpdateActionBinding(const FInputAction& SrcInputAction, FInputActionKeyMapping& DstInputAction)
{
	DstInputAction.Key = SrcInputAction.Key;
	DstInputAction.bShift = SrcInputAction.bShift;
	DstInputAction.bCtrl = SrcInputAction.bCtrl;
	DstInputAction.bAlt = SrcInputAction.bAlt;
	DstInputAction.bCmd = SrcInputAction.bCmd;
}

/*
*
*/
FInputAction UKeyBindingUtil::GetInputActionFromMouseButtonEvent(const FPointerEvent& MouseEvent)
{
	FInputAction InputAction;

	InputAction.Key = MouseEvent.GetEffectingButton();
	InputAction.KeyAsString = InputAction.Key.GetDisplayName().ToString();

	InputAction.bAlt = MouseEvent.IsAltDown();
	InputAction.bCtrl = MouseEvent.IsControlDown();
	InputAction.bShift = MouseEvent.IsShiftDown();
	InputAction.bCmd = MouseEvent.IsCommandDown();

	return InputAction;
}

/*
*
*/
FInputAxis UKeyBindingUtil::GetInputAxisFromMouseButtonEvent(const FPointerEvent& MouseEvent)
{
	FInputAxis InputAxis;

	InputAxis.Key = MouseEvent.GetEffectingButton();
	InputAxis.KeyAsString = InputAxis.Key.GetDisplayName().ToString();

	InputAxis.Scale = 1;

	return InputAxis;
}

/*
*
*/
FInputAxis UKeyBindingUtil::GetInputAxisFromMouseAxisEvent(const FPointerEvent& MouseEvent)
{
	FInputAxis InputAxis;

	float CurrentHorizontalDistance = FMath::Abs(MouseEvent.GetCursorDelta().X);
	float CurrentVerticalDistance = FMath::Abs(MouseEvent.GetCursorDelta().Y);

	if (CurrentHorizontalDistance > CurrentVerticalDistance)
	{
		InputAxis.Key = FKey(FName("MouseX"));

		InputAxis.Scale = MouseEvent.GetCursorDelta().X >= 0.f ? 1 : -1;
	}
	else
	{
		InputAxis.Key = FKey(FName("MouseY"));

		InputAxis.Scale = MouseEvent.GetCursorDelta().Y >= 0.f ? 1 : -1;
	}


	InputAxis.KeyAsString = InputAxis.Key.GetDisplayName().ToString();

	return InputAxis;
}

/*
*
*/
FInputAction UKeyBindingUtil::GetInputActionFromKeyEvent(const FKeyEvent& KeyEvent)
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

/*
*
*/
FInputAxis UKeyBindingUtil::GetInputAxisFromKeyEvent(const FKeyEvent& KeyEvent)
{
	FInputAxis InputAxis;

	InputAxis.Key = KeyEvent.GetKey();
	InputAxis.KeyAsString = InputAxis.Key.GetDisplayName().ToString();

	InputAxis.Scale = 1;

	return InputAxis;
}

/*
*
*/
void UKeyBindingUtil::GetAllBindedInputActions(TArray<FInputAction>& InputActions)
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

/*
*
*/
void UKeyBindingUtil::GetAllBindedInputAxis(TArray<FInputAxis>& InputAxis)
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

/*
*
*/
bool UKeyBindingUtil::ReBindAxisKey(FInputAxis CurrentBinding, FInputAxis NewBinding)
{
	UInputSettings* Settings = const_cast<UInputSettings*>(GetDefault<UInputSettings>());
	if (!Settings) return false;

	TArray<FInputAxisKeyMapping>& Axis = Settings->AxisMappings;

	bool Found = false;
	for (FInputAxisKeyMapping& Each : Axis)
	{
		//Search by CurrentBinding
		if (Each.AxisName.ToString() == CurrentBinding.AxisName && Each.Key == CurrentBinding.Key) {
			UKeyBindingUtil::UpdateAxisBinding(NewBinding, Each);
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

/*
*
*/
bool UKeyBindingUtil::ReBindActionKey(FInputAction CurrentBinding, FInputAction NewBinding)
{
	UInputSettings* Settings = const_cast<UInputSettings*>(GetDefault<UInputSettings>());
	if (!Settings) return false;

	TArray<FInputActionKeyMapping>& Actions = Settings->ActionMappings;

	bool Found = false;
	for (FInputActionKeyMapping& Each : Actions)
	{
		if (Each.ActionName.ToString() == CurrentBinding.ActionName && Each.Key == CurrentBinding.Key) {
			UKeyBindingUtil::UpdateActionBinding(NewBinding, Each);
			Found = true;
			break;
		}
	}

	if (Found)
	{
		const_cast<UInputSettings*>(Settings)->SaveKeyMappings();

		for (TObjectIterator<UPlayerInput> It; It; ++It)
		{
			It->ForceRebuildingKeyMaps(true);
		}
	}

	return Found;
}

/*
*
*/
bool UKeyBindingUtil::AddAxisBinding(FInputAxis NewBinding, FInputAxis CurrentBinding)
{
	UInputSettings* Settings = const_cast<UInputSettings*>(GetDefault<UInputSettings>());
	if (!Settings) return false;

	TArray<FInputAxisKeyMapping>& Axis = Settings->AxisMappings;	

	int32 ActionIndex = Axis.AddUnique(FInputAxisKeyMapping(FName(*CurrentBinding.AxisName), NewBinding.Key, NewBinding.Scale));

	bool bOk = ActionIndex > 0 ? true : false;

	if (bOk)
	{
		const_cast<UInputSettings*>(Settings)->SaveKeyMappings();

		for (TObjectIterator<UPlayerInput> It; It; ++It)
		{
			It->ForceRebuildingKeyMaps(true);
		}
	}

	return bOk;
}

/*
*
*/
bool UKeyBindingUtil::AddActionBinding(FInputAction NewBinding, FInputAction CurrentBinding)
{
	UInputSettings* Settings = const_cast<UInputSettings*>(GetDefault<UInputSettings>());
	if (!Settings) return false;

	TArray<FInputActionKeyMapping>& Actions = Settings->ActionMappings;
	
	int32 ActionIndex = Actions.AddUnique(FInputActionKeyMapping(FName(*CurrentBinding.ActionName), NewBinding.Key, NewBinding.bShift, NewBinding.bCtrl, NewBinding.bAlt, NewBinding.bCmd));

	bool bOk = ActionIndex > 0 ? true : false;

	if (bOk)
	{
		const_cast<UInputSettings*>(Settings)->SaveKeyMappings();

		for (TObjectIterator<UPlayerInput> It; It; ++It)
		{
			It->ForceRebuildingKeyMaps(true);
		}
	}

	return bOk;
}

/*
*
*/
bool UKeyBindingUtil::RemoveAxisBinding(FInputAxis BindingToRemove)
{
	UInputSettings* Settings = GetMutableDefault<UInputSettings>();
	if (!Settings) return false;

	TArray<FInputAxisKeyMapping>& Axi = Settings->AxisMappings;

	bool Found = false;
	for (int32 v = 0; v < Axi.Num(); v++)
	{
		if (Axi[v].Key == BindingToRemove.Key)
		{
			Found = true;
			Axi.RemoveAt(v);
			v = 0;
			continue;
		}
	}

	if (Found)
	{
		Settings->SaveKeyMappings();

		for (TObjectIterator<UPlayerInput> It; It; ++It)
		{
			It->ForceRebuildingKeyMaps(true);
		}
	}

	return Found;
}

/*
*
*/
bool UKeyBindingUtil::RemoveActionBinding(FInputAction BindingToRemove)
{
	UInputSettings* Settings = GetMutableDefault<UInputSettings>();
	if (!Settings) return false;

	TArray<FInputActionKeyMapping>& Actions = Settings->ActionMappings;

	bool Found = false;
	for (int32 v = 0; v < Actions.Num(); v++)
	{
		if (Actions[v].Key == BindingToRemove.Key)
		{
			Found = true;
			Actions.RemoveAt(v);
			v = 0;
			continue;
		}
	}

	if (Found)
	{
		Settings->SaveKeyMappings();

		for (TObjectIterator<UPlayerInput> It; It; ++It)
		{
			It->ForceRebuildingKeyMaps(true);
		}
	}

	return Found;
}

