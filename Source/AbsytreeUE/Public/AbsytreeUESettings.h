#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AbsytreeUESettings.generated.h"

/**
 * Contains general settings for the Absytree plugin
 */
UCLASS(config = "Absytree")
class ABSYTREEUE_API UAbsytreeUESettings : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "Absytree", meta = (FilePathFilter = "*.dll|*.so"))
	FFilePath DllPath = {"D:/Absytree/ast.dll"};

	/** How often to poll absytree (necessary for async stuff). */
	UPROPERTY(Config, EditAnywhere, Category = "Absytree", meta = (Units = "s"))
	float PollInterval = 0.25f;

	/** Timeout for polling absytree. */
	UPROPERTY(Config, EditAnywhere, Category = "Absytree", meta = (Units = "ms"))
	float PollTimeoutMs = 10;
};
