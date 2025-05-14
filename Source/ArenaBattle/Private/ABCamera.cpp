// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCamera.h"

// Sets default values
AABCamera::AABCamera()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
}

// Called when the game starts or when spawned
void AABCamera::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AABCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

