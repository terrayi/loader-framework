/*
 * Loader Framework
 *
 * - Author  : t3RRa
 * - License : 2-clause BSD license (see LICENSE file)
 * - File    : loader_utility_defs.h
 * - Description
 *   : This defines required enumerations, macros and function
 *     to compile under MinGW32 which lacks some. :)
 */

#include <windows.h>

#ifndef _MSC_VER

#ifndef _LOADER_UTILITY_DEFS_H
#define _LOADER_UTILITY_DEFS_H

typedef enum  {
	TokenElevationTypeDefault = 1,
	TokenElevationTypeFull,
	TokenElevationTypeLimited 
} TOKEN_ELEVATION_TYPE, *PTOKEN_ELEVATION_TYPE;

#define SECURITY_MAX_SID_SIZE 68
#define TokenElevationType 19
#define TokenLinkedToken   20
#define TokenElevation     21

/*
typedef enum _TOKEN_INFORMATION_CLASS {
	TokenUser = 1,
	TokenGroups,
	TokenPrivileges,
	TokenOwner,
	TokenPrimaryGroup,
	TokenDefaultDacl,
	TokenSource,
	TokenType,
	TokenImpersonationLevel,
	TokenStatistics,
	TokenRestrictedSids,
	TokenSessionId,
	TokenGroupsAndPrivileges,
	TokenSessionReference,
	TokenSandBoxInert,
	TokenAuditPolicy,
	TokenOrigin,
	TokenElevationType,
	TokenLinkedToken,
	TokenElevation,
	TokenHasRestrictions,
	TokenAccessInformation,
	TokenVirtualizationAllowed,
	TokenVirtualizationEnabled,
	TokenIntegrityLevel,
	TokenUIAccess,
	TokenMandatoryPolicy,
	TokenLogonSid,
	MaxTokenInfoClass 
}; TOKEN_INFORMATION_CLASS, *PTOKEN_INFORMATION_CLASS;
*/
typedef struct _TOKEN_ELEVATION {
	DWORD TokenIsElevated;
} TOKEN_ELEVATION, *PTOKEN_ELEVATION;

typedef enum  {
  WinNullSid                                    = 0,
  WinWorldSid                                   = 1,
  WinLocalSid                                   = 2,
  WinCreatorOwnerSid                            = 3,
  WinCreatorGroupSid                            = 4,
  WinCreatorOwnerServerSid                      = 5,
  WinCreatorGroupServerSid                      = 6,
  WinNtAuthoritySid                             = 7,
  WinDialupSid                                  = 8,
  WinNetworkSid                                 = 9,
  WinBatchSid                                   = 10,
  WinInteractiveSid                             = 11,
  WinServiceSid                                 = 12,
  WinAnonymousSid                               = 13,
  WinProxySid                                   = 14,
  WinEnterpriseControllersSid                   = 15,
  WinSelfSid                                    = 16,
  WinAuthenticatedUserSid                       = 17,
  WinRestrictedCodeSid                          = 18,
  WinTerminalServerSid                          = 19,
  WinRemoteLogonIdSid                           = 20,
  WinLogonIdsSid                                = 21,
  WinLocalSystemSid                             = 22,
  WinLocalServiceSid                            = 23,
  WinNetworkServiceSid                          = 24,
  WinBuiltinDomainSid                           = 25,
  WinBuiltinAdministratorsSid                   = 26,
  WinBuiltinUsersSid                            = 27,
  WinBuiltinGuestsSid                           = 28,
  WinBuiltinPowerUsersSid                       = 29,
  WinBuiltinAccountOperatorsSid                 = 30,
  WinBuiltinSystemOperatorsSid                  = 31,
  WinBuiltinPrintOperatorsSid                   = 32,
  WinBuiltinBackupOperatorsSid                  = 33,
  WinBuiltinReplicatorSid                       = 34,
  WinBuiltinPreWindows2000CompatibleAccessSid   = 35,
  WinBuiltinRemoteDesktopUsersSid               = 36,
  WinBuiltinNetworkConfigurationOperatorsSid    = 37,
  WinAccountAdministratorSid                    = 38,
  WinAccountGuestSid                            = 39,
  WinAccountKrbtgtSid                           = 40,
  WinAccountDomainAdminsSid                     = 41,
  WinAccountDomainUsersSid                      = 42,
  WinAccountDomainGuestsSid                     = 43,
  WinAccountComputersSid                        = 44,
  WinAccountControllersSid                      = 45,
  WinAccountCertAdminsSid                       = 46,
  WinAccountSchemaAdminsSid                     = 47,
  WinAccountEnterpriseAdminsSid                 = 48,
  WinAccountPolicyAdminsSid                     = 49,
  WinAccountRasAndIasServersSid                 = 50,
  WinNTLMAuthenticationSid                      = 51,
  WinDigestAuthenticationSid                    = 52,
  WinSChannelAuthenticationSid                  = 53,
  WinThisOrganizationSid                        = 54,
  WinOtherOrganizationSid                       = 55,
  WinBuiltinIncomingForestTrustBuildersSid      = 56,
  WinBuiltinPerfMonitoringUsersSid              = 57,
  WinBuiltinPerfLoggingUsersSid                 = 58,
  WinBuiltinAuthorizationAccessSid              = 59,
  WinBuiltinTerminalServerLicenseServersSid     = 60,
  WinBuiltinDCOMUsersSid                        = 61,
  WinBuiltinIUsersSid                           = 62,
  WinIUserSid                                   = 63,
  WinBuiltinCryptoOperatorsSid                  = 64,
  WinUntrustedLabelSid                          = 65,
  WinLowLabelSid                                = 66,
  WinMediumLabelSid                             = 67,
  WinHighLabelSid                               = 68,
  WinSystemLabelSid                             = 69,
  WinWriteRestrictedCodeSid                     = 70,
  WinCreatorOwnerRightsSid                      = 71,
  WinCacheablePrincipalsGroupSid                = 72,
  WinNonCacheablePrincipalsGroupSid             = 73,
  WinEnterpriseReadonlyControllersSid           = 74,
  WinAccountReadonlyControllersSid              = 75,
  WinBuiltinEventLogReadersGroup                = 76,
  WinNewEnterpriseReadonlyControllersSid        = 77,
  WinBuiltinCertSvcDComAccessGroup              = 78 
} WELL_KNOWN_SID_TYPE;

/*
  It needs CreateWellKnownSid() in windows/wine library to link properly.
  Unless, it won't compile at all with MinGW32.. :(
*/
typedef BOOL (WINAPI *fnCWKStype)(WELL_KNOWN_SID_TYPE, PSID, PSID, DWORD*);

/*
BOOL WINAPI CreateWellKnownSid(
	WELL_KNOWN_SID_TYPE WellKnownSidType,
	PSID DomainSid,
	PSID pSid,
	DWORD *cbSid) = NULL;
*/

fnCWKStype CreateWellKnownSid;

#endif

#endif

//EOF
