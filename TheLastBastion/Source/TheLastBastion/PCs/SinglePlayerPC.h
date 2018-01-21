// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SinglePlayerPC.generated.h"

/**
 * 
 */
UCLASS()
class THELASTBASTION_API ASinglePlayerPC : public APlayerController
{
	GENERATED_BODY()
	
public:

	ASinglePlayerPC(const FObjectInitializer & _objectInit);
	
private:

	UPROPERTY()
		TSubclassOf<UUserWidget> InGameHuD_WBPClass;
	UPROPERTY()
		class UInGameHUD*                  mInGameHUD;


	UPROPERTY()
		TSubclassOf<UUserWidget> SmithShop_WBPClass;
	UPROPERTY()
		class UUserWidget*                 mSmithShopHUD;

	
	struct FPlayerProfile   playerProfile;

public:



	UFUNCTION()
		void OnPostLogin();

	UFUNCTION()
		void OnFinishSeamlessTravel();

	UFUNCTION()
		void InitUIOnBeginPlay(const class UHeroStatsComponent* _heroStats);

	UFUNCTION()
		void OnHealthChange(const class UPawnStatsComponent* _heroStats);

	UFUNCTION(Exec)
		void OpenSmithShop();

	UFUNCTION(Exec)
		void CloseSmithShop();

	FORCEINLINE class UInGameHUD* GetInGameHUD() const { return mInGameHUD; }

private:

	void SaveGameCheck();

	void CreateInGameHUD();
};
