// Fill out your copyright notice in the Description page of Project Settings.

#include "SaveGame_TheLastBastion.h"

const FPlayerProfile * USaveGame_TheLastBastion::GetPlayerProfile() const
{
	return &mPlayerProfile;
}

void USaveGame_TheLastBastion::SetPlayerProfile(const FPlayerProfile& _in)
{
	mPlayerProfile = _in;
}
