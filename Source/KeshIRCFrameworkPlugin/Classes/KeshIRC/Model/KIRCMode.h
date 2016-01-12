// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#pragma once

#include "KeshIRC/KIRCTypes.h"
#include "KIRCMode.generated.h"


/**
* Representation of a user on an IRC server.
* Should be treated as singleton instances generated by the server.
*/
UCLASS( ClassGroup = "KeshIRC|Model|Model", Blueprintable, BlueprintType )
class KESHIRCFRAMEWORKPLUGIN_API UKIRCMode : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION( Category = "KeshIRC|Model|Mode", BlueprintCallable )
	const FString& GetMode() const { return Mode; }

	UFUNCTION( Category = "KeshIRC|Model|Mode", BlueprintCallable )
	EKIRCModeType GetType() const { return Type; }

	UFUNCTION( Category = "KeshIRC|Model|Mode", BlueprintCallable )
	EKIRCModeParamRequired GetParamRequired() const { return ParamRequired; }

	UFUNCTION( Category = "KeshIRC|Model|Mode", BlueprintCallable )
	bool RequireParamForChange( EKIRCModeChange ModeChange ) const;

protected:

	friend class UKIRCServer;

	UPROPERTY( Category = "KeshIRC|Model|Mode", VisibleInstanceOnly )
	FString Mode;

	UPROPERTY( Category = "KeshIRC|Model|Mode", VisibleInstanceOnly )
	EKIRCModeType Type;

	UPROPERTY( Category = "KeshIRC|Model|Mode", VisibleInstanceOnly )
	EKIRCModeParamRequired ParamRequired;

	UKIRCMode( const class FObjectInitializer& ObjectInitializer );

	UFUNCTION( Category = "KeshIRC|Model|Mode", BlueprintCallable )
	virtual void InitMode( const FString& ModeCharacter, EKIRCModeType ModeType, EKIRCModeParamRequired ModeParamRequired = EKIRCModeParamRequired::R_Never )
	{ 
		this->Mode = ModeCharacter;
		this->Type = ModeType;
		this->ParamRequired = ModeParamRequired;
	}

};
