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
		TSubclassOf<UUserWidget> TradeMenu_WBPClass;
	UPROPERTY()
		class UTradeMenu*                 mTradeMenu;
	UPROPERTY()
		TSubclassOf<UUserWidget> InGameMenu_WBPClass;
	UPROPERTY()
		class UInGameMenu*                mInGameMenu;

	UPROPERTY()
		TSubclassOf<UUserWidget> MeleeRecruitMenu_WBPClass;
	UPROPERTY()
		class URecruitMenu*               mMeleeRecruitMenu;

	UPROPERTY()
		TSubclassOf<UUserWidget> RangeRecruitMenu_WBPClass;
	UPROPERTY()
		class URecruitMenu*               mRangeRecruitMenu;
	
	struct FPlayerProfile   playerProfile;

public:

	UFUNCTION()
		void OnPostLogin();

	UFUNCTION()
		void OnFinishSeamlessTravel();

	UFUNCTION()
		void OnPauseButtonIsPressed();

	UFUNCTION()
		void OnHealthChange(const class UPawnStatsComponent* _heroStats);

	UFUNCTION(Exec)
		void OpenTradeMenu();

	UFUNCTION(Exec)
		void OpenMeleeRecruitMenu();

	UFUNCTION(Exec)
		void OpenRangeRecruitMenu();


	UFUNCTION(Exec)
		void CloseTradeMenu();

	UFUNCTION(Exec)
		void ShowFrontLine();

	UFUNCTION(Exec)
		void ShowColumnAt(int _index);

	void OnSelectedCrewAt(int _index);



	/** Update the stats based on the change of gears*/
	void OnTradeMenuAccept(class UHeroStatsComponent* _heroStats);

public:

	FORCEINLINE class UInGameHUD* GetInGameHUD() const { return mInGameHUD; }

private:

	void SaveGameCheck();

	void CreateInGameHUD();

	void OpenInGameMenu();

	void SetInputModeForOpenMenu(class UUserWidget* _focus);



};
