// Copyright 2014-2015 Matthew Chapman, Inc. All Rights Reserved.

#include "Private/KeshIRCFrameworkPluginPrivatePCH.h"
#include "KeshIRC/Model/KIRCServer.h"
#include "KeshIRC/Controller/KIRCClient.h"
#include "KeshIRC/Model/KIRCUser.h"
#include "KeshIRC/Model/KIRCChannel.h"


UKIRCChannel::UKIRCChannel( const class FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	TopicBody = "";
	TopicAuthor = "";
	TopicDate = FDateTime( 0 );
	Created = FDateTime( 0 );
	UnaryModes.SetNum( 0 );
	ListModes.Empty();
	Limit = 0;
	Users.Empty();
}


bool UKIRCChannel::HasChannelPrefix( const FString& ObjectName )
{
	if ( ObjectName.Len() == 0 )
		return false;
	
	return ( ObjectName[ 0 ] == '#' || ObjectName[ 0 ] == '&' || ObjectName[ 0 ] == '+' || ObjectName[ 0 ] == '!' );
}


void UKIRCChannel::InitChannel( const FString& Name )
{
	InitObject( Name );
}


const FKIRCChannelUserInfo& UKIRCChannel::GetChannelUserInfo( const UKIRCUser* const User ) const
{
	static const FKIRCChannelUserInfo DefaultReturnValue = { NULL, NULL, FDateTime( 0 ), TArray<UKIRCMode*>() };
	
	if ( User == NULL )
	{
		KIRCLog( Error, "Trying to get channel user info for a null user." );
		return DefaultReturnValue;
	}

	if ( !Users.Contains( User ) )
		return DefaultReturnValue;

	return Users[ User ];
}


bool UKIRCChannel::HasUserLimit() const
{
	const UKIRCServer* const Server = GetServer();

	if ( Server == NULL )
	{
		KIRCLog( Error, "Cannot get server." );
		return false;
	}

	const UKIRCMode* const KeyMode = Server->GetChannelMode( UKIRCClient::GetModes().Channel.UserLimit );

	if ( KeyMode == NULL )
		return false;

	return IsChannelUnaryModeSet( KeyMode );
}


bool UKIRCChannel::IsJoinKeySet() const
{
	const UKIRCServer* const Server = GetServer();

	if ( Server == NULL )
	{
		KIRCLog( Error, "Cannot get server." );
		return false;
	}

	const UKIRCMode* const KeyMode = Server->GetChannelMode( UKIRCClient::GetModes().Channel.Key );

	if ( KeyMode == NULL )
		return false;

	return IsChannelParamModeSet( KeyMode );
}


const FString& UKIRCChannel::GetJoinKey() const
{
	static FString strDefaultReturnValue = "";

	UKIRCServer* Server = GetServer();

	if ( Server == NULL )
		return strDefaultReturnValue;

	const UKIRCMode* const KeyMode = Server->GetChannelMode( UKIRCClient::GetModes().Channel.Key );

	return GetChannelParamModeValue( KeyMode );
}


const FString& UKIRCChannel::GetChannelParamModeValue( const UKIRCMode* const Mode ) const
{
	static FString strDefaultReturnValue = "";

	if ( Mode == NULL )
		return strDefaultReturnValue;

	if ( !ParamModes.Contains( Mode ) )
		return strDefaultReturnValue;

	return ParamModes[ Mode ];
}


const TArray<FString>& UKIRCChannel::GetChannelListModeEntries( const UKIRCMode* const Mode ) const
{
	static const TArray<FString> DefaultReturnValue = TArray<FString>();

	if ( Mode == NULL )
	{
		KIRCLog( Error, "Trying to channel mode list for null mode." );
		return DefaultReturnValue;
	}

	if ( !ListModes.Contains( Mode ) )
		return DefaultReturnValue;

	return ListModes[ Mode ].List;
}


void UKIRCChannel::UserJoined( UKIRCUser* const User, bool bZeroTime )
{
	if ( User == NULL )
	{
		KIRCLog( Error, "Null user trying to join a channel." );
		return;
	}

	if ( HasUser( User ) )
	{
		KIRCLog( Error, "User joining channel that they are already in." );
		return;
	}

	FKIRCChannelUserInfo CUI = { this, User, bZeroTime ? FDateTime( 0 ) : FDateTime::Now(), TArray<UKIRCMode*>() };
	Users.Emplace( User, CUI );
	User->JoinChannel( this );
}


void UKIRCChannel::UserLeft( UKIRCUser* const User )
{
	if ( User == NULL )
	{
		KIRCLog( Error, "Null user trying to leave a channel." );
		return;
	}

	if ( !HasUser( User ) )
	{
		KIRCLog( Error, "User leaving channel that they are not in." );
		return;
	}

	Users.Remove( User );
	User->LeaveChannel( this );
}


void UKIRCChannel::AddUnaryMode( const UKIRCMode* const Mode )
{
	if ( Mode == NULL )
	{
		KIRCLog( Error, "Trying to add a null unary mode." );
		return;
	}

	if ( IsChannelUnaryModeSet( Mode ) )
	{
		KIRCLog( Error, "Trying to add a unary mode that's already set." );
		return;
	}

	UnaryModes.AddUnique( const_cast< UKIRCMode* >( Mode ) );
}


void UKIRCChannel::RemoveUnaryMode( const UKIRCMode* const Mode )
{
	if ( Mode == NULL )
	{
		KIRCLog( Error, "Trying to remove a null unary mode." );
		return;
	}

	if ( !IsChannelUnaryModeSet( Mode ) )
	{
		KIRCLog( Error, "Trying to remove a unary mode that isn't set." );
		return;
	}

	UnaryModes.Remove( const_cast< UKIRCMode* >( Mode ) );
}


void UKIRCChannel::SetParamMode( const UKIRCMode* const Mode, const FString& Value )
{
	if ( Mode == NULL )
	{
		KIRCLog( Error, "Trying to set a null param mode." );
		return;
	}

	if ( Value.Len() == 0 )
	{
		KIRCLog( Error, "Trying to set a zero-length param mode value." );
		return;
	}

	ParamModes.Emplace( const_cast< UKIRCMode* >( Mode ), Value );
}


void UKIRCChannel::RemoveParamMode( const UKIRCMode* const Mode )
{
	if ( Mode == NULL )
	{
		KIRCLog( Error, "Trying to remove a null param mode." );
		return;
	}

	if ( !ParamModes.Contains( Mode ) )
		return;

	ParamModes.Remove( Mode );
}


void UKIRCChannel::AddListModeEntry( const UKIRCMode* const Mode, const FString& Entry )
{
	if ( Mode == NULL )
	{
		KIRCLog( Error, "Trying to add a null list mode." );
		return;
	}

	if ( Entry.Len() == 0 )
	{
		KIRCLog( Error, "Trying to add a zero length list mode value." );
		return;
	}

	if ( !ListModes.Contains( Mode ) )
	{
		FKIRCModeListContainer Entries;
		ListModes.Emplace( const_cast< UKIRCMode* >( Mode ), Entries );
	}

	ListModes[ Mode ].List.Add( Entry );
}



void UKIRCChannel::RemoveListModeEntry( const UKIRCMode* const Mode, const FString& Entry )
{
	if ( Mode == NULL )
	{
		KIRCLog( Error, "Trying to remove a null list mode." );
		return;
	}

	if ( Entry.Len() == 0 )
	{
		KIRCLog( Error, "Trying to remove a zero length list mode value." );
		return;
	}

	if ( !ListModes.Contains( Mode ) )
	{
		KIRCLog( Error, "Trying to remove a list mode value that has none set." );
		return;
	}

	ListModes[ Mode ].List.Remove( Entry );

	if ( ListModes[ Mode ].List.Num() == 0 )
		ListModes.Remove( Mode );
}


void UKIRCChannel::AddUserMode( const UKIRCUser* const User, const UKIRCMode* const Mode )
{
	if ( User == NULL )
	{
		KIRCLog( Error, "Trying to add a mode to a null user." );
		return;
	}
	
	if ( Mode == NULL )
	{
		KIRCLog( Error, "Trying add a nul mode to a user." );
		return;
	}

	if ( !Users.Contains( User ) )
	{
		KIRCLog( Error, "Trying add a mode to a user not in the channel." );
		return;
	}

	Users[ User ].Modes.AddUnique( const_cast< UKIRCMode* >( Mode ) );
}


void UKIRCChannel::RemoveUserMode( const UKIRCUser* const User, const UKIRCMode* const Mode )
{
	if ( User == NULL )
	{
		KIRCLog( Error, "Trying to remove a mode to a null user." );
		return;
	}

	if ( Mode == NULL )
	{
		KIRCLog( Error, "Trying remove a nul mode to a user." );
		return;
	}

	if ( !Users.Contains( User ) )
	{
		KIRCLog( Error, "Trying remove a mode to a user not in the channel." );
		return;
	}

	Users[ User ].Modes.Remove( const_cast< UKIRCMode* >( Mode ) );
}
