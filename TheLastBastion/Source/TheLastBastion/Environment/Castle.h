// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Castle.generated.h"

UCLASS(BlueprintType)
class THELASTBASTION_API ACastle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACastle();

protected:

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = CastleDefence)
		/** Enemies that Inside the castle */
		TArray<class ATheLastBastionAIBase*> Enemies;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = CastleDefence)
		/** Box Volumn to check if there is enemy breach the castle*/
		class UBoxComponent* CastleBox;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = CastleDefence)
		class UStaticMeshComponent* CastleMesh;

	/** Timer to control the castle update*/
	FTimerHandle HealthTimer;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = CastleDefence)
		/** Time gap to check on castle health*/
		float UpdateFreq;

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

private:

	/** Check castle health based on how many enemies inside the castle*/
	void UpdateCastleHealth();

	/** Called when lived enemies inside the castle*/
	void OnEnemiesEnter(class ATheLastBastionAIBase* _enemy);

	/** Called when enemies leave the castle*/
	void OnEnemiesLeave(class ATheLastBastionAIBase* _enemy);

	UFUNCTION()
		void OnCastleBoxOverlap_Start(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
		void OnCastleBoxOverlap_End(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
