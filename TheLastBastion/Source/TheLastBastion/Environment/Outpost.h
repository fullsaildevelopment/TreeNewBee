// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Outpost.generated.h"

UENUM(BlueprintType)
enum class EOutpostType : uint8
{
	None = 0                  UMETA(DisplayName = "None"),
	Food = 1                  UMETA(DisplayName = "Food"),
	Wood = 2                  UMETA(DisplayName = "Wood"),
	Metal = 3                 UMETA(DisplayName = "Metal"),
	Stone = 4                 UMETA(DisplayName = "Stone"),
	Castle = 5                UMETA(DisplayName = "Castle")
};

#define FoodIncrement 10
#define StoneIncrement 10
#define MetalIncrement 6
#define WoodIncrement 8


UCLASS()
class THELASTBASTION_API AOutpost : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOutpost();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



protected:

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Outpost)
		/** Enemy group that overrlap the occupy area */
		TArray<class AAIGroupBase*> EnemiesGroup;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Outpost)
		/** Box Volumn to check if there is enemy group in occupy area*/
		class UBoxComponent* OccupyVolumn;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Outpost)
		class UStaticMeshComponent* OutpostMesh;

	/** Timer to control the outpost update*/
	FTimerHandle UpdateTimer;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Outpost)
		/** Time gap to check on if we should produce any resource*/
		float UpdateFreq;


	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Outpost)
		/** Addition Amount in unit value, that will add to each increment*/
		float AdditionAmount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Outpost)
		EOutpostType OutpostType;


	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Outpost)
		/** if true, the outpost is occupied by enemies, and will not produce resource for hero*/
		bool bIsOccupied;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Outpost)
		/** if true, the outpost is occupied by enemies, and will not produce resource for hero*/
		bool bDisabled;



protected:

	/** Called every UpdateFreq sec, and check whether the outpost should produce resource for hero*/
	virtual void UpdateByTimer();


	UFUNCTION()
		virtual void OnOutPostBoxOverlap_Start(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
		virtual void OnOutPostBoxOverlap_End(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);



public:

	FORCEINLINE EOutpostType GetOutpostType() const { return OutpostType; }
	FORCEINLINE float GetAdditionAmount() const { return AdditionAmount; }
	FORCEINLINE void SetAdditionAmount( float _val) {  AdditionAmount = _val; }

};
