// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_SharedDecorator.generated.h"

/**
 * 
 */


UENUM(BlueprintType)

enum class EAISharedDecoratorType : uint8
{
	None = 0                        UMETA(DisplayName = "None"),
	AttackConditionCheck = 1        UMETA(DisplayName = "AttackConditionCheck")
};

UCLASS()
class THELASTBASTION_API UBTDecorator_SharedDecorator : public UBTDecorator
{
	GENERATED_BODY()
	
public:

	virtual void SetOwner(AActor* ActorOwner) override;




protected:

	UPROPERTY(EditAnywhere, Category = SharedDecorator)
		EAISharedDecoratorType sharedDecorator;

	/** Cached AIController owner of BehaviorTreeComponent. */
	UPROPERTY(Transient)
		class ATheLastBastionBaseAIController* AIOwner;


	/** called when underlying node is activated
	* this function should be considered as const (don't modify state of object) if node is not instanced! */
	virtual void OnNodeActivation(FBehaviorTreeSearchData& SearchData) override;

	/** called when underlying node has finished
	* this function should be considered as const (don't modify state of object) if node is not instanced! */
	virtual void OnNodeDeactivation(FBehaviorTreeSearchData& SearchData, EBTNodeResult::Type NodeResult) override;

	/** called when underlying node was processed (deactivated or failed to activate)
	* this function should be considered as const (don't modify state of object) if node is not instanced! */
	virtual void OnNodeProcessed(FBehaviorTreeSearchData& SearchData, EBTNodeResult::Type& NodeResult) override;

	/** calculates raw, core value of decorator's condition. Should not include calling IsInversed */
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;



private:


	
	bool AttackConditionCheck_CalculateRawConidtionValue(UBehaviorTreeComponent& OwnerComp) const;
	void AttackConditionCheck_ExecutionFinish();

};
