// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Environment/Outpost.h"
#include "Castle.generated.h"



UCLASS(BlueprintType)
class THELASTBASTION_API ACastle : public AOutpost
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACastle();

protected:


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CastleDefence)
		class UInGameHUD* GameHUD;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = CastleDefence)
		/** Max value of castle hp*/
		float MaxHp;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = CastleDefence)
		float MaxHpDiv;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = CastleDefence)
		/** Current value of castle hp*/
		float CurrentHp;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = CastleDefence)
		/** The percentage of damage is applied to castle due to commander presence*/
		float DamageReduction;



	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = CastleDefence)
		/** Command presence skilll is enabled, it help to reduce the damage caused by enemies*/
		bool bCommanderPresence;



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void OnWaveFinished(class ASinglePlayerGM* _gm);

	void SetIsOccupied(bool _val);

protected:

	/** Check castle health based on how many enemies inside the castle*/
	void UpdateByTimer() override;

	UFUNCTION()
		void OnOutPostBoxOverlap_Start(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, 
			const FHitResult & SweepResult) override;

	UFUNCTION()
		void OnOutPostBoxOverlap_End(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor, UPrimitiveComponent* OtherComp, 
			int32 OtherBodyIndex) override;



};
