// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CustomType.h"
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
		TSubclassOf<UUserWidget> RecruitMenu_WBPClass;
	UPROPERTY()
		class URecruitMenu*               mRecruitMenu;

	UPROPERTY()	
		struct FPlayerProfile   playerProfile;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		AActor* playerSpawnPoint;


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
		void OpenRecruitMenu();

	UFUNCTION(Exec)
		void LevelUp(int _deltaLevel);

	UFUNCTION(Exec)
		void GainSkillPoints(int _val);

	UFUNCTION(Exec)
		void CloseTradeMenu();

	UFUNCTION(Exec)
		void ShowFrontLine();

	UFUNCTION(Exec)
		void ShowColumnAt(int _index);

	UFUNCTION(Exec)
		void ForTheNightIsDarkAndFullOfTerrors(int _val);

	void OnSelectedCrewAt(int _index);


	/** Update the stats based on the change of gears*/
	void OnTradeMenuAccept(class UHeroStatsComponent* _heroStats);

public:

	FORCEINLINE AActor* GetPlayerStart() const { return playerSpawnPoint; }
	FORCEINLINE class UInGameHUD* GetInGameHUD() const { return mInGameHUD; }
	FORCEINLINE class UTradeMenu* GetTradeMenu() const { return mTradeMenu; }
	FORCEINLINE class URecruitMenu* GetRecruitMenu() const { return mRecruitMenu; }

	class UInventoryUI* GetInventoryUI() const;


private:

	void SaveGameCheck();

	void CreateInGameHUD();

	void OpenInGameMenu();

	void SetInputModeForOpenMenu(class UUserWidget* _focus);



};
