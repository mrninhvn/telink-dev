/********************************************************************************************************
 * @file     zcl_wwah.c
 *
 * @brief	 APIs for WWAH cluster
 *
 * @author
 * @date     June. 10, 2017
 *
 * @par      Copyright (c) 2016, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *			 The information contained herein is confidential and proprietary property of Telink
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in.
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/


/**********************************************************************
 * INCLUDES
 */
#include "../zcl_include.h"


#ifdef ZCL_WWAH
/**********************************************************************
 * LOCAL CONSTANTS
 */


/**********************************************************************
 * LOCAL TYPES
 */
typedef struct{
	ev_time_event_t *periodicRouterCheckInEvt;
	epInfo_t	dstEpInfo;
	u16			manufCode;
	u8			srcEp;
}wwah_perRouterCheckInInfo_t;

/**********************************************************************
 * LOCAL VARIABLES
 */
wwah_apsLinkKeyAuthInfo_t g_zcl_apsLinkKeyAuthInfo;
wwah_apsAcksRequireInfo_t g_zcl_apsAcksRequireInfo;
wwah_useTCForClusterInfo_t g_zcl_useTCForClusterInfo;
wwah_perRouterCheckInInfo_t g_zcl_perRouterCheckInInfo;
bool rejoinFlag = FALSE;

/**********************************************************************
 * LOCAL FUNCTIONS
 */
static void zcl_wwah_apsLinkKeyAuthInfoClear(void);
static nv_sts_t zcl_wwah_apsLinkKeyAuthInfoRestore(void);
static void zcl_wwah_apsAcksRequireInfoClear(void);
static nv_sts_t zcl_wwah_apsAcksRequireInfoRestore(void);
static void zcl_wwah_useTCForClusterInfoClear(void);
static nv_sts_t zcl_wwah_useTCForClusterInfoRestore(void);
static status_t zcl_wwah_cmdHandler(zclIncoming_t *pInMsg);


_CODE_ZCL_ status_t zcl_wwah_register(u8 endpoint, u8 attrNum, const zclAttrInfo_t attrTbl[], cluster_forAppCb_t cb)
{
    if(zcl_wwah_apsLinkKeyAuthInfoRestore() != NV_SUCC){
    	zcl_wwah_apsLinkKeyAuthInfoClear();
    }
    if(zcl_wwah_apsAcksRequireInfoRestore() != NV_SUCC){
    	zcl_wwah_apsAcksRequireInfoClear();
    }
	if(zcl_wwah_useTCForClusterInfoRestore() != NV_SUCC){
		zcl_wwah_useTCForClusterInfoClear();
	}

	return zcl_registerCluster(endpoint, ZCL_CLUSTER_WWAH, attrNum, attrTbl, zcl_wwah_cmdHandler, cb);
}

_CODE_ZCL_ static void zcl_wwah_apsLinkKeyAuthInfoClear(void)
{
	g_zcl_apsLinkKeyAuthInfo.status = APS_LINK_KEY_AUTH_DISABLE;
	g_zcl_apsLinkKeyAuthInfo.num = 0;
	memset((u8 *)g_zcl_apsLinkKeyAuthInfo.clusterID, 0, sizeof(u16) * CLUSTER_ID_MAX_NUM);
}

_CODE_ZCL_ static nv_sts_t zcl_wwah_apsLinkKeyAuthInfoRestore(void)
{
	return zcl_apsLinkKeyAuthInfo_restore();
}

_CODE_ZCL_ static nv_sts_t zcl_wwah_apsLinkKeyAuthInfoSave(void *arg)
{
	return zcl_apsLinkKeyAuthInfo_save();
}

_CODE_ZCL_ static void zcl_wwah_apsAcksRequireInfoClear(void)
{
	g_zcl_apsAcksRequireInfo.num = 0;
	memset((u8 *)g_zcl_apsAcksRequireInfo.clusterID, 0, sizeof(u16) * CLUSTER_ID_MAX_NUM);
}

_CODE_ZCL_ static nv_sts_t zcl_wwah_apsAcksRequireInfoRestore(void)
{
	return zcl_apsAcksRequireInfo_restore();
}

_CODE_ZCL_ static nv_sts_t zcl_wwah_apsAcksRequireInfoSave(void *arg)
{
	return zcl_apsAcksRequireInfo_save();
}

_CODE_ZCL_ static void zcl_wwah_useTCForClusterInfoClear(void)
{
	g_zcl_useTCForClusterInfo.num = 0;
	memset((u8 *)g_zcl_useTCForClusterInfo.clusterID, 0, sizeof(u16) * CLUSTER_ID_MAX_NUM);
}

_CODE_ZCL_ static nv_sts_t zcl_wwah_useTCForClusterInfoRestore(void)
{
	return zcl_useTCForClusterInfo_restore();
}

_CODE_ZCL_ static nv_sts_t zcl_wwah_useTCForClusterInfoSave(void *arg)
{
	return zcl_useTCForClusterInfo_save();
}

_CODE_ZCL_ bool zcl_wwah_apsLinkKeyAuthCheck(u16 clusterID, bool apsUseSec)
{
	bool accept;

	if(g_zcl_apsLinkKeyAuthInfo.status == APS_LINK_KEY_AUTH_DISABLE){
		accept = TRUE;

		for(u8 i = 0; i < g_zcl_apsLinkKeyAuthInfo.num; i++){
			if((g_zcl_apsLinkKeyAuthInfo.clusterID[i] == clusterID) && !apsUseSec){
				accept = FALSE;
				break;
			}
		}
	}else{
		if(apsUseSec){
			return TRUE;
		}

		accept = FALSE;

		for(u8 i = 0; i < g_zcl_apsLinkKeyAuthInfo.num; i++){
			if(g_zcl_apsLinkKeyAuthInfo.clusterID[i] == clusterID){
				accept = TRUE;
				break;
			}
		}
	}

	return accept;
}

_CODE_ZCL_ static apsLinkKeyAuthStatus_t zcl_wwah_apsLinkKeyAuthSearch(u16 clusterID)
{
	u8 find = 0;
	for(u8 i = 0; i < g_zcl_apsLinkKeyAuthInfo.num; i++){
		if(g_zcl_apsLinkKeyAuthInfo.clusterID[i] == clusterID){
			find = 1;
			break;
		}
	}

	return (find ? (!g_zcl_apsLinkKeyAuthInfo.status) : g_zcl_apsLinkKeyAuthInfo.status);
}

_CODE_ZCL_ static void zcl_wwah_rejoin(void *arg)
{
	zb_rejoinReq(NLME_REJOIN_METHOD_REJOIN, zb_apsChannelMaskGet());
}

_CODE_ZCL_ static s32 periodicRouterCheckInCb(void *arg)
{
	static u16 cnt = 0;
	u16 checkInInterval = (u16)((u32)arg);

	if(++cnt >= checkInInterval){
		cnt = 0;

		if(rejoinFlag){
			TL_SCHEDULE_TASK(zcl_wwah_rejoin, NULL);

			g_zcl_perRouterCheckInInfo.periodicRouterCheckInEvt = NULL;
			return -1;
		}

		zclReadCmd_t *pReadCmd = (zclReadCmd_t *)ev_buf_allocate(sizeof(zclReadCmd_t) + sizeof(u16));
		if(pReadCmd){
			pReadCmd->numAttr = 1;
			pReadCmd->attrID[0] = ZCL_ATTRID_GLOBAL_CLUSTER_REVISION;

			zcl_sendReadWithMfgCodeCmd( g_zcl_perRouterCheckInInfo.srcEp, &g_zcl_perRouterCheckInInfo.dstEpInfo,
										ZCL_CLUSTER_GEN_BASIC, g_zcl_perRouterCheckInInfo.manufCode,
										FALSE, ZCL_FRAME_CLIENT_SERVER_DIR, pReadCmd );

			ev_buf_free((u8 *)pReadCmd);

			rejoinFlag = TRUE;
		}
	}

	return 0;
}

_CODE_ZCL_ void zcl_wwah_periodicRouterChecked(void)
{
	rejoinFlag = FALSE;
}

_CODE_ZCL_ bool zcl_wwah_apsAckRequirementCheck(u16 clusterID)
{
	for(u8 i = 0; i < g_zcl_apsAcksRequireInfo.num; i++){
		if(g_zcl_apsAcksRequireInfo.clusterID[i] == clusterID){
			return FALSE;
		}
	}
	return TRUE;
}

_CODE_ZCL_ bool zcl_wwah_useTCForClusterCheck(u16 clusterID)
{
	for(u8 i = 0; i < g_zcl_useTCForClusterInfo.num; i++){
		if(g_zcl_useTCForClusterInfo.clusterID[i] == clusterID){
			return TRUE;
		}
	}
	return FALSE;
}

_CODE_ZCL_ bool zcl_wwah_touchlinkEnabled(zclIncoming_t *pInMsg)
{
	bool enable = TRUE;
	u16 len;

	if(zcl_getAttrVal(pInMsg->msg->indInfo.dst_ep, ZCL_CLUSTER_WWAH, ZCL_ATTRID_WWAH_TOUCHLINK_INTERPAN_ENABLED, &len, (u8 *)&enable) == ZCL_STA_SUCCESS){
		return enable;
	}
	return TRUE;
}

_CODE_ZCL_ static void zcl_wwah_reqNewApsLinkKey(void *arg)
{
	ss_apsmeRequestKeyReq(SS_KEYREQ_TYPE_TCLK, ss_ib.trust_center_address, NULL);
}


_CODE_ZCL_ status_t zcl_wwah_enableApsLinkKeyAuthCmd(u8 srcEp, epInfo_t *pDstEpInfo, u8 disableDefaultRsp, u8 seqNo, clustersToExempt_t *pCmd)
{
	u8 status = ZCL_STA_SUCCESS;
	u8 len = 1 + pCmd->numOfClustersToExempt * sizeof(u16);

	u8 *buf = (u8 *)ev_buf_allocate(len);
	if(!buf){
		return ZCL_STA_INSUFFICIENT_SPACE;
	}

	u8 *pBuf = buf;

	*pBuf++ = pCmd->numOfClustersToExempt;
	for(u8 i = 0; i < pCmd->numOfClustersToExempt; i++){
		*pBuf++ = LO_UINT16(pCmd->pClusterID[i]);
		*pBuf++ = HI_UINT16(pCmd->pClusterID[i]);
	}

	status = zcl_sendCmd(srcEp, pDstEpInfo, ZCL_CLUSTER_WWAH, ZCL_CMD_WWAH_ENABLE_APS_LINK_KEY_AUTH, TRUE,
    					ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNo, len, buf);

    ev_buf_free(buf);

    return status;
}

_CODE_ZCL_ status_t zcl_wwah_disableApsLinkKeyAuthCmd(u8 srcEp, epInfo_t *pDstEpInfo, u8 disableDefaultRsp, u8 seqNo, clustersToExempt_t *pCmd)
{
	u8 status = ZCL_STA_SUCCESS;
	u8 len = 1 + pCmd->numOfClustersToExempt * sizeof(u16);

	u8 *buf = (u8 *)ev_buf_allocate(len);
	if(!buf){
		return ZCL_STA_INSUFFICIENT_SPACE;
	}

	u8 *pBuf = buf;

	*pBuf++ = pCmd->numOfClustersToExempt;
	for(u8 i = 0; i < pCmd->numOfClustersToExempt; i++){
		*pBuf++ = LO_UINT16(pCmd->pClusterID[i]);
		*pBuf++ = HI_UINT16(pCmd->pClusterID[i]);
	}

	status = zcl_sendCmd(srcEp, pDstEpInfo, ZCL_CLUSTER_WWAH, ZCL_CMD_WWAH_DISABLE_APS_LINK_KEY_AUTH, TRUE,
    					ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNo, len, buf);

    ev_buf_free(buf);

    return status;
}

_CODE_ZCL_ status_t zcl_wwah_apsLinkKeyAuthQueryCmd(u8 srcEp, epInfo_t *pDstEpInfo, u8 disableDefaultRsp, u8 seqNo, zcl_wwah_apsLinkKeyAuthQueryCmd_t *pCmd)
{
	u8 buf[2];
	u8 *pBuf = buf;

	*pBuf++ = LO_UINT16(pCmd->clusterID);
	*pBuf++ = HI_UINT16(pCmd->clusterID);

	return zcl_sendCmd(srcEp, pDstEpInfo, ZCL_CLUSTER_WWAH, ZCL_CMD_WWAH_APS_LINK_KEY_AUTH_QUERY, TRUE,
    					ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNo, 2, buf);
}

_CODE_ZCL_ status_t zcl_wwah_requestNewApsLinkKeyCmd(u8 srcEp, epInfo_t *pDstEpInfo, u8 disableDefaultRsp, u8 seqNo)
{
	return zcl_sendCmd(srcEp, pDstEpInfo, ZCL_CLUSTER_WWAH, ZCL_CMD_WWAH_REQUEST_NEW_APS_LINK_KEY, TRUE,
    					ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNo, 0, NULL);
}

_CODE_ZCL_ status_t zcl_wwah_enableAppEvtRetryAlgorithmCmd(u8 srcEp, epInfo_t *pDstEpInfo, u8 disableDefaultRsp, u8 seqNo, zcl_wwah_enableAppEvtRetryAlgorithmCmd_t *pCmd)
{
	u8 buf[7];
	u8 *pBuf = buf;

	*pBuf++ = pCmd->firstBackoffTimeInSeconds;
	*pBuf++ = pCmd->backoffSequenceCommonRatio;
	*pBuf++ = U32_BYTE0(pCmd->maxBackoffTimeInSeconds);
	*pBuf++ = U32_BYTE1(pCmd->maxBackoffTimeInSeconds);
	*pBuf++ = U32_BYTE2(pCmd->maxBackoffTimeInSeconds);
	*pBuf++ = U32_BYTE3(pCmd->maxBackoffTimeInSeconds);
	*pBuf++ = pCmd->maxReDeliveryAttepts;

	return zcl_sendCmd(srcEp, pDstEpInfo, ZCL_CLUSTER_WWAH, ZCL_CMD_WWAH_ENABLE_APP_EVT_RETRY_ALGORITHM, TRUE,
    					ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNo, 7, buf);
}

_CODE_ZCL_ status_t zcl_wwah_disableAppEvtRetryAlgorithmCmd(u8 srcEp, epInfo_t *pDstEpInfo, u8 disableDefaultRsp, u8 seqNo)
{
	return zcl_sendCmd(srcEp, pDstEpInfo, ZCL_CLUSTER_WWAH, ZCL_CMD_WWAH_DISABLE_APP_EVT_RETRY_ALGORITHM, TRUE,
    					ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNo, 0, NULL);
}

_CODE_ZCL_ status_t zcl_wwah_requestTimeCmd(u8 srcEp, epInfo_t *pDstEpInfo, u8 disableDefaultRsp, u8 seqNo)
{
	return zcl_sendCmd(srcEp, pDstEpInfo, ZCL_CLUSTER_WWAH, ZCL_CMD_WWAH_REQUEST_TIME, TRUE,
    					ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNo, 0, NULL);
}

_CODE_ZCL_ status_t zcl_wwah_enableRejoinAlgorithmCmd(u8 srcEp, epInfo_t *pDstEpInfo, u8 disableDefaultRsp, u8 seqNo, zcl_wwah_enableRejoinAlgorithmCmd_t *pCmd)
{
	u8 buf[10];
	u8 *pBuf = buf;

	*pBuf++ = LO_UINT16(pCmd->fastRejoinTimeoutInSeconds);
	*pBuf++ = HI_UINT16(pCmd->fastRejoinTimeoutInSeconds);
	*pBuf++ = LO_UINT16(pCmd->durationBetweenEachRejoinInSeconds);
	*pBuf++ = HI_UINT16(pCmd->durationBetweenEachRejoinInSeconds);
	*pBuf++ = LO_UINT16(pCmd->fastRejoinFirstBackoffInSeconds);
	*pBuf++ = HI_UINT16(pCmd->fastRejoinFirstBackoffInSeconds);
	*pBuf++ = LO_UINT16(pCmd->maxBackoffTimeInSeconds);
	*pBuf++ = HI_UINT16(pCmd->maxBackoffTimeInSeconds);
	*pBuf++ = LO_UINT16(pCmd->maxBackoffIterations);
	*pBuf++ = HI_UINT16(pCmd->maxBackoffIterations);

	return zcl_sendCmd(srcEp, pDstEpInfo, ZCL_CLUSTER_WWAH, ZCL_CMD_WWAH_ENABLE_REJOIN_ALGORITHM, TRUE,
    					ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNo, 10, buf);
}

_CODE_ZCL_ status_t zcl_wwah_disableRejoinAlgorithmCmd(u8 srcEp, epInfo_t *pDstEpInfo, u8 disableDefaultRsp, u8 seqNo)
{
	return zcl_sendCmd(srcEp, pDstEpInfo, ZCL_CLUSTER_WWAH, ZCL_CMD_WWAH_DISABLE_REJOIN_ALGORITHM, TRUE,
    					ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNo, 0, NULL);
}

_CODE_ZCL_ status_t zcl_wwah_setIasZoneEnrollmentMethodCmd(u8 srcEp, epInfo_t *pDstEpInfo, u8 disableDefaultRsp, u8 seqNo, zcl_wwah_setIasZoneEnrollmentMethodCmd_t *pCmd)
{
	u8 buf[1];

	buf[0] = (u8)pCmd->enrollmentMode;

	return zcl_sendCmd(srcEp, pDstEpInfo, ZCL_CLUSTER_WWAH, ZCL_CMD_WWAH_SET_IAS_ZONE_ENROLLMENT_METHOD, TRUE,
    					ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNo, 1, buf);
}

_CODE_ZCL_ status_t zcl_wwah_clearBindingTableCmd(u8 srcEp, epInfo_t *pDstEpInfo, u8 disableDefaultRsp, u8 seqNo)
{
	return zcl_sendCmd(srcEp, pDstEpInfo, ZCL_CLUSTER_WWAH, ZCL_CMD_WWAH_CLEAR_BINDING_TABLE, TRUE,
    					ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNo, 0, NULL);
}

_CODE_ZCL_ status_t zcl_wwah_enablePeriodicRouterCheckInsCmd(u8 srcEp, epInfo_t *pDstEpInfo, u8 disableDefaultRsp, u8 seqNo, zcl_wwah_enablePeriodicRouterCheckInsCmd_t *pCmd)
{
	u8 buf[2];
	u8 *pBuf = buf;

	*pBuf++ = LO_UINT16(pCmd->checkInInterval);
	*pBuf++ = HI_UINT16(pCmd->checkInInterval);

	return zcl_sendCmd(srcEp, pDstEpInfo, ZCL_CLUSTER_WWAH, ZCL_CMD_WWAH_ENABLE_PERIODIC_ROUTER_CHECK_INS, TRUE,
    					ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNo, 2, buf);
}

_CODE_ZCL_ status_t zcl_wwah_disablePeriodicRouterCheckInsCmd(u8 srcEp, epInfo_t *pDstEpInfo, u8 disableDefaultRsp, u8 seqNo)
{
	return zcl_sendCmd(srcEp, pDstEpInfo, ZCL_CLUSTER_WWAH, ZCL_CMD_WWAH_DISABLE_PERIODIC_ROUTER_CHECK_INS, TRUE,
    					ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNo, 0, NULL);
}

_CODE_ZCL_ status_t zcl_wwah_setMacPollFailureWaitTimeCmd(u8 srcEp, epInfo_t *pDstEpInfo, u8 disableDefaultRsp, u8 seqNo, zcl_wwah_setMacPollFailureWaitTimeCmd_t *pCmd)
{
	u8 buf[1];

	buf[0] = pCmd->waitTime;

	return zcl_sendCmd(srcEp, pDstEpInfo, ZCL_CLUSTER_WWAH, ZCL_CMD_WWAH_SET_MAC_POLL_FAILURE_WAIT_TIME, TRUE,
    					ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNo, 1, buf);
}

_CODE_ZCL_ status_t zcl_wwah_setPendingNwkUpdateCmd(u8 srcEp, epInfo_t *pDstEpInfo, u8 disableDefaultRsp, u8 seqNo, zcl_wwah_setPendingNwkUpdateCmd_t *pCmd)
{
	u8 buf[3];
	u8 *pBuf = buf;

	*pBuf++ = pCmd->channel;
	*pBuf++ = LO_UINT16(pCmd->panID);
	*pBuf++ = HI_UINT16(pCmd->panID);

	return zcl_sendCmd(srcEp, pDstEpInfo, ZCL_CLUSTER_WWAH, ZCL_CMD_WWAH_SET_PENDING_NWK_UPDATE, TRUE,
    					ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNo, 3, buf);
}

_CODE_ZCL_ status_t zcl_wwah_requireApsAcksOnUnicastsCmd(u8 srcEp, epInfo_t *pDstEpInfo, u8 disableDefaultRsp, u8 seqNo, clustersToExempt_t *pCmd)
{
	u8 status = ZCL_STA_SUCCESS;
	u8 len = 1 + pCmd->numOfClustersToExempt * sizeof(u16);

	u8 *buf = (u8 *)ev_buf_allocate(len);
	if(!buf){
		return ZCL_STA_INSUFFICIENT_SPACE;
	}

	u8 *pBuf = buf;

	*pBuf++ = pCmd->numOfClustersToExempt;
	for(u8 i = 0; i < pCmd->numOfClustersToExempt; i++){
		*pBuf++ = LO_UINT16(pCmd->pClusterID[i]);
		*pBuf++ = HI_UINT16(pCmd->pClusterID[i]);
	}

	status = zcl_sendCmd(srcEp, pDstEpInfo, ZCL_CLUSTER_WWAH, ZCL_CMD_WWAH_REQUIRE_APS_ACKS_ON_UNICASTS, TRUE,
    					ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNo, len, buf);

    ev_buf_free(buf);

    return status;
}

_CODE_ZCL_ status_t zcl_wwah_removeApsAcksOnUnicastsRequirementCmd(u8 srcEp, epInfo_t *pDstEpInfo, u8 disableDefaultRsp, u8 seqNo)
{
	return zcl_sendCmd(srcEp, pDstEpInfo, ZCL_CLUSTER_WWAH, ZCL_CMD_WWAH_REMOVE_APS_ACKS_ON_UNICASTS_REQUIREMENT, TRUE,
    					ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNo, 0, NULL);
}

_CODE_ZCL_ status_t zcl_wwah_apsAckRequirementQueryCmd(u8 srcEp, epInfo_t *pDstEpInfo, u8 disableDefaultRsp, u8 seqNo)
{
	return zcl_sendCmd(srcEp, pDstEpInfo, ZCL_CLUSTER_WWAH, ZCL_CMD_WWAH_APS_ACK_REQUIREMENT_QUERY, TRUE,
    					ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNo, 0, NULL);
}

_CODE_ZCL_ status_t zcl_wwah_debugReportQueryCmd(u8 srcEp, epInfo_t *pDstEpInfo, u8 disableDefaultRsp, u8 seqNo, zcl_wwah_debugReportQueryCmd_t *pCmd)
{
	u8 buf[1];

	buf[0] = pCmd->debugReportID;

	return zcl_sendCmd(srcEp, pDstEpInfo, ZCL_CLUSTER_WWAH, ZCL_CMD_WWAH_DEBUG_REPORT_QUERY, TRUE,
    					ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNo, 1, buf);
}

_CODE_ZCL_ status_t zcl_wwah_surveyBeaconsCmd(u8 srcEp, epInfo_t *pDstEpInfo, u8 disableDefaultRsp, u8 seqNo, zcl_wwah_surveyBeaconsCmd_t *pCmd)
{
	u8 buf[1];

	buf[0] = pCmd->standardBeacons;

	return zcl_sendCmd(srcEp, pDstEpInfo, ZCL_CLUSTER_WWAH, ZCL_CMD_WWAH_SURVEY_BEACONS, TRUE,
    					ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNo, 1, buf);
}

_CODE_ZCL_ status_t zcl_wwah_disableOtaDowngradesCmd(u8 srcEp, epInfo_t *pDstEpInfo, u8 disableDefaultRsp, u8 seqNo)
{
	return zcl_sendCmd(srcEp, pDstEpInfo, ZCL_CLUSTER_WWAH, ZCL_CMD_WWAH_DISABLE_OTA_DOWNGRADES, TRUE,
    					ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNo, 0, NULL);
}

_CODE_ZCL_ status_t zcl_wwah_disableMgmtLeaveWithoutRejoinCmd(u8 srcEp, epInfo_t *pDstEpInfo, u8 disableDefaultRsp, u8 seqNo)
{
	return zcl_sendCmd(srcEp, pDstEpInfo, ZCL_CLUSTER_WWAH, ZCL_CMD_WWAH_DISABLE_MGMT_LEAVE_WITHOUT_REJOIN, TRUE,
    					ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNo, 0, NULL);
}

_CODE_ZCL_ status_t zcl_wwah_disableTouchlinkInterpanMsgCmd(u8 srcEp, epInfo_t *pDstEpInfo, u8 disableDefaultRsp, u8 seqNo)
{
	return zcl_sendCmd(srcEp, pDstEpInfo, ZCL_CLUSTER_WWAH, ZCL_CMD_WWAH_DISABLE_TOUCHLINK_INTERPAN_MSG_SUPPORT, TRUE,
    					ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNo, 0, NULL);
}

_CODE_ZCL_ status_t zcl_wwah_enableParentClassificationCmd(u8 srcEp, epInfo_t *pDstEpInfo, u8 disableDefaultRsp, u8 seqNo)
{
	return zcl_sendCmd(srcEp, pDstEpInfo, ZCL_CLUSTER_WWAH, ZCL_CMD_WWAH_ENABLE_PARENT_CLASSIFICATION, TRUE,
    					ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNo, 0, NULL);
}

_CODE_ZCL_ status_t zcl_wwah_disableParentClassificationCmd(u8 srcEp, epInfo_t *pDstEpInfo, u8 disableDefaultRsp, u8 seqNo)
{
	return zcl_sendCmd(srcEp, pDstEpInfo, ZCL_CLUSTER_WWAH, ZCL_CMD_WWAH_DISABLE_PARENT_CLASSIFICATION, TRUE,
    					ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNo, 0, NULL);
}

_CODE_ZCL_ status_t zcl_wwah_enableTcSecurityOnNwkKeyRotationCmd(u8 srcEp, epInfo_t *pDstEpInfo, u8 disableDefaultRsp, u8 seqNo)
{
	return zcl_sendCmd(srcEp, pDstEpInfo, ZCL_CLUSTER_WWAH, ZCL_CMD_WWAH_ENABLE_TC_SECURITY_ON_NWK_KEY_ROTATION, TRUE,
    					ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNo, 0, NULL);
}

_CODE_ZCL_ status_t zcl_wwah_enableBadParentRecoveryCmd(u8 srcEp, epInfo_t *pDstEpInfo, u8 disableDefaultRsp, u8 seqNo)
{
	return zcl_sendCmd(srcEp, pDstEpInfo, ZCL_CLUSTER_WWAH, ZCL_CMD_WWAH_ENABLE_BAD_PARENT_RECOVERY, TRUE,
    					ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNo, 0, NULL);
}

_CODE_ZCL_ status_t zcl_wwah_disableBadParentRecoveryCmd(u8 srcEp, epInfo_t *pDstEpInfo, u8 disableDefaultRsp, u8 seqNo)
{
	return zcl_sendCmd(srcEp, pDstEpInfo, ZCL_CLUSTER_WWAH, ZCL_CMD_WWAH_DISABLE_BAD_PARENT_RECOVERY, TRUE,
    					ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNo, 0, NULL);
}

_CODE_ZCL_ status_t zcl_wwah_enableConfigurationModeCmd(u8 srcEp, epInfo_t *pDstEpInfo, u8 disableDefaultRsp, u8 seqNo)
{
	return zcl_sendCmd(srcEp, pDstEpInfo, ZCL_CLUSTER_WWAH, ZCL_CMD_WWAH_ENABLE_CONFIGURATION_MODE, TRUE,
    					ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNo, 0, NULL);
}

_CODE_ZCL_ status_t zcl_wwah_disableConfigurationModeCmd(u8 srcEp, epInfo_t *pDstEpInfo, u8 disableDefaultRsp, u8 seqNo)
{
	return zcl_sendCmd(srcEp, pDstEpInfo, ZCL_CLUSTER_WWAH, ZCL_CMD_WWAH_DISABLE_CONFIGURATION_MODE, TRUE,
    					ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNo, 0, NULL);
}

_CODE_ZCL_ status_t zcl_wwah_useTrustCenterForClusterCmd(u8 srcEp, epInfo_t *pDstEpInfo, u8 disableDefaultRsp, u8 seqNo, zcl_wwah_useTrustCenterForClusterCmd_t *pCmd)
{
	u8 status = ZCL_STA_SUCCESS;
	u8 len = 1 + pCmd->numOfClusters * sizeof(u16);

	u8 *buf = (u8 *)ev_buf_allocate(len);
	if(!buf){
		return ZCL_STA_INSUFFICIENT_SPACE;
	}

	u8 *pBuf = buf;

	*pBuf++ = pCmd->numOfClusters;
	for(u8 i = 0; i < pCmd->numOfClusters; i++){
		*pBuf++ = LO_UINT16(pCmd->pClusterID[i]);
		*pBuf++ = HI_UINT16(pCmd->pClusterID[i]);
	}

	status = zcl_sendCmd(srcEp, pDstEpInfo, ZCL_CLUSTER_WWAH, ZCL_CMD_WWAH_USE_TRUST_CENTER_FOR_CLUSTER, TRUE,
    					ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNo, len, buf);

    ev_buf_free(buf);

    return status;
}

_CODE_ZCL_ status_t zcl_wwah_trustCenterForClusterServerQueryCmd(u8 srcEp, epInfo_t *pDstEpInfo, u8 disableDefaultRsp, u8 seqNo)
{
	return zcl_sendCmd(srcEp, pDstEpInfo, ZCL_CLUSTER_WWAH, ZCL_CMD_WWAH_TRUST_CENTER_FOR_CLUSTER_SERVER_QUERY, TRUE,
    					ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNo, 0, NULL);
}

_CODE_ZCL_ status_t zcl_wwah_apsLinkKeyAuthQueryRspCmd(u8 srcEp, epInfo_t *pDstEpInfo, u16 manuCode, u8 disableDefaultRsp, u8 seqNo, zcl_wwah_apsLinkKeyAuthQueryRspCmd_t *pCmd)
{
	u8 buf[3];
	u8 *pBuf = buf;

	*pBuf++ = LO_UINT16(pCmd->clusterID);
	*pBuf++ = HI_UINT16(pCmd->clusterID);
	*pBuf++ = pCmd->status;

    return zcl_sendCmd(srcEp, pDstEpInfo, ZCL_CLUSTER_WWAH, ZCL_CMD_WWAH_APS_LINK_KEY_AUTH_QUERY_RSP, TRUE,
    		ZCL_FRAME_SERVER_CLIENT_DIR, disableDefaultRsp, manuCode, seqNo, 3, buf);
}

_CODE_ZCL_ status_t zcl_wwah_poweringOffNotificationCmd(u8 srcEp, epInfo_t *pDstEpInfo, u8 disableDefaultRsp, u8 seqNo, zcl_wwah_poweringNotificationCmd_t *pCmd)
{
	u8 status = ZCL_STA_SUCCESS;
	u8 len = 4 + pCmd->manuPowerNotiReasonLen;

	u8 *buf = (u8 *)ev_buf_allocate(len);
	if(!buf){
		return ZCL_STA_INSUFFICIENT_SPACE;
	}

	u8 *pBuf = buf;

	*pBuf++ = (u8)pCmd->powerNotiReason;
	*pBuf++ = LO_UINT16(pCmd->manuID);
	*pBuf++ = HI_UINT16(pCmd->manuID);
	*pBuf++ = pCmd->manuPowerNotiReasonLen;
	if(pCmd->manuPowerNotiReasonLen){
		memcpy(pBuf, pCmd->pManuPowerNotiReason, pCmd->manuPowerNotiReasonLen);
		pBuf += pCmd->manuPowerNotiReasonLen;
	}

	status = zcl_sendCmd(srcEp, pDstEpInfo, ZCL_CLUSTER_WWAH, ZCL_CMD_WWAH_POWERING_OFF_NOTIFICATION, TRUE,
    		ZCL_FRAME_SERVER_CLIENT_DIR, disableDefaultRsp, 0, seqNo, len, buf);

    ev_buf_free(buf);

    return status;
}

_CODE_ZCL_ status_t zcl_wwah_poweringOnNotificationCmd(u8 srcEp, epInfo_t *pDstEpInfo, u8 disableDefaultRsp, u8 seqNo, zcl_wwah_poweringNotificationCmd_t *pCmd)
{
	u8 status = ZCL_STA_SUCCESS;
	u8 len = 4 + pCmd->manuPowerNotiReasonLen;

	u8 *buf = (u8 *)ev_buf_allocate(len);
	if(!buf){
		return ZCL_STA_INSUFFICIENT_SPACE;
	}

	u8 *pBuf = buf;

	*pBuf++ = (u8)pCmd->powerNotiReason;
	*pBuf++ = LO_UINT16(pCmd->manuID);
	*pBuf++ = HI_UINT16(pCmd->manuID);
	*pBuf++ = pCmd->manuPowerNotiReasonLen;
	if(pCmd->manuPowerNotiReasonLen){
		memcpy(pBuf, pCmd->pManuPowerNotiReason, pCmd->manuPowerNotiReasonLen);
		pBuf += pCmd->manuPowerNotiReasonLen;
	}

	status = zcl_sendCmd(srcEp, pDstEpInfo, ZCL_CLUSTER_WWAH, ZCL_CMD_WWAH_POWERING_ON_NOTIFICATION, TRUE,
    		ZCL_FRAME_SERVER_CLIENT_DIR, disableDefaultRsp, 0, seqNo, len, buf);

    ev_buf_free(buf);

    return status;
}

_CODE_ZCL_ status_t zcl_wwah_shortAddrChangeCmd(u8 srcEp, epInfo_t *pDstEpInfo, u8 disableDefaultRsp, u8 seqNo, zcl_wwah_shortAddrChangeCmd_t *pCmd)
{
	u8 buf[10];
	u8 *pBuf = buf;

	ZB_64BIT_ADDR_COPY(pBuf, pCmd->deviceEUI64);
	pBuf += EXT_ADDR_LEN;
	*pBuf++ = LO_UINT16(pCmd->deviceShort);
	*pBuf++ = HI_UINT16(pCmd->deviceShort);

    return zcl_sendCmd(srcEp, pDstEpInfo, ZCL_CLUSTER_WWAH, ZCL_CMD_WWAH_SHORT_ADDRESS_CHANGE, TRUE,
    		ZCL_FRAME_SERVER_CLIENT_DIR, disableDefaultRsp, 0, seqNo, 10, buf);
}

_CODE_ZCL_ status_t zcl_wwah_apsAckRequirementQueryRspCmd(u8 srcEp, epInfo_t *pDstEpInfo, u16 manuCode, u8 disableDefaultRsp, u8 seqNo, clustersToExempt_t *pCmd)
{
	u8 status = ZCL_STA_SUCCESS;
	u8 len = 1 + pCmd->numOfClustersToExempt * sizeof(u16);

	u8 *buf = (u8 *)ev_buf_allocate(len);
	if(!buf){
		return ZCL_STA_INSUFFICIENT_SPACE;
	}

	u8 *pBuf = buf;

	*pBuf++ = pCmd->numOfClustersToExempt;
	for(u8 i = 0; i < pCmd->numOfClustersToExempt; i++){
		*pBuf++ = LO_UINT16(pCmd->pClusterID[i]);
		*pBuf++ = HI_UINT16(pCmd->pClusterID[i]);
	}

	status = zcl_sendCmd(srcEp, pDstEpInfo, ZCL_CLUSTER_WWAH, ZCL_CMD_WWAH_APS_ACK_REQUIREMENT_QUERY_RSP, TRUE,
    		ZCL_FRAME_SERVER_CLIENT_DIR, disableDefaultRsp, manuCode, seqNo, len, buf);

    ev_buf_free(buf);

    return status;
}

_CODE_ZCL_ status_t zcl_wwah_powerDescriptorChangeCmd(u8 srcEp, epInfo_t *pDstEpInfo, u8 disableDefaultRsp, u8 seqNo, zcl_wwah_powerDescChangeCmd_t *pCmd)
{
	u8 buf[2];
	u8 *pBuf = buf;

	*pBuf++ = (pCmd->currentPowerMode << 4) + pCmd->availablePowerSrcs;
	*pBuf++ = (pCmd->currentPowerSrc << 4) + pCmd->currentPowerSrcLevel;

    return zcl_sendCmd(srcEp, pDstEpInfo, ZCL_CLUSTER_WWAH, ZCL_CMD_WWAH_POWER_DESCRIPTOR_CHANGE, TRUE,
    		ZCL_FRAME_SERVER_CLIENT_DIR, disableDefaultRsp, 0, seqNo, 2, buf);
}

_CODE_ZCL_ status_t zcl_wwah_newDebugReportNotificationCmd(u8 srcEp, epInfo_t *pDstEpInfo, u8 disableDefaultRsp, u8 seqNo, zcl_wwah_newDbgReportNotiCmd_t *pCmd)
{
	u8 buf[5];
	u8 *pBuf = buf;

	*pBuf++ = pCmd->dbgReportID;
	*pBuf++ = U32_BYTE0(pCmd->sizeOfReport);
	*pBuf++ = U32_BYTE1(pCmd->sizeOfReport);
	*pBuf++ = U32_BYTE2(pCmd->sizeOfReport);
	*pBuf++ = U32_BYTE3(pCmd->sizeOfReport);

    return zcl_sendCmd(srcEp, pDstEpInfo, ZCL_CLUSTER_WWAH, ZCL_CMD_WWAH_NEW_DEBUG_REPORT_NOTIFICATION, TRUE,
    		ZCL_FRAME_SERVER_CLIENT_DIR, disableDefaultRsp, 0, seqNo, 5, buf);
}

_CODE_ZCL_ status_t zcl_wwah_debugReportQueryRspCmd(u8 srcEp, epInfo_t *pDstEpInfo, u16 manuCode, u8 disableDefaultRsp, u8 seqNo, zcl_wwah_dbgReportQueryRspCmd_t *pCmd)
{
	u8 status = ZCL_STA_SUCCESS;
	u8 len = 1 + pCmd->reportDataLen;

	u8 *buf = (u8 *)ev_buf_allocate(len);
	if(!buf){
		return ZCL_STA_INSUFFICIENT_SPACE;
	}

	u8 *pBuf = buf;

	*pBuf++ = pCmd->dbgReportID;
	*pBuf++ = pCmd->reportDataLen;
	if(pCmd->reportDataLen){
		memcpy(pBuf, pCmd->pReportData, pCmd->reportDataLen);
		pBuf += pCmd->reportDataLen;
	}

	status = zcl_sendCmd(srcEp, pDstEpInfo, ZCL_CLUSTER_WWAH, ZCL_CMD_WWAH_DEBUG_REPORT_QUERY_RSP, TRUE,
    		ZCL_FRAME_SERVER_CLIENT_DIR, disableDefaultRsp, manuCode, seqNo, len, buf);

    ev_buf_free(buf);

    return status;
}

_CODE_ZCL_ status_t zcl_wwah_trustCenterForClusterServerQueryRspCmd(u8 srcEp, epInfo_t *pDstEpInfo, u16 manufCode, u8 disableDefaultRsp, u8 seqNo, zcl_wwah_tcForClusterServerQueryRspCmd_t *pCmd)
{
	u8 status = ZCL_STA_SUCCESS;
	u8 len = 1 + pCmd->numOfClusters * sizeof(u16);

	u8 *buf = (u8 *)ev_buf_allocate(len);
	if(!buf){
		return ZCL_STA_INSUFFICIENT_SPACE;
	}

	u8 *pBuf = buf;

	*pBuf++ = pCmd->numOfClusters;
	for(u8 i = 0; i < pCmd->numOfClusters; i++){
		*pBuf++ = LO_UINT16(pCmd->pClusterID[i]);
		*pBuf++ = HI_UINT16(pCmd->pClusterID[i]);
	}

	status = zcl_sendCmd(srcEp, pDstEpInfo, ZCL_CLUSTER_WWAH, ZCL_CMD_WWAH_TRUST_CENTER_FOR_CLUSTER_SERVER_QUERY_RSP, TRUE,
    		ZCL_FRAME_SERVER_CLIENT_DIR, disableDefaultRsp, manufCode, seqNo, len, buf);

    ev_buf_free(buf);

    return status;
}

_CODE_ZCL_ status_t zcl_wwah_surveyBeaconsRspCmd(u8 srcEp, epInfo_t *pDstEpInfo, u8 disableDefaultRsp, u8 seqNo, zcl_wwah_surveyBeaconsRspCmd_t *pCmd)
{
	u8 status = ZCL_STA_SUCCESS;
	u8 len = 1 + pCmd->numOfBeacons * 4;

	u8 *buf = (u8 *)ev_buf_allocate(len);
	if(!buf){
		return ZCL_STA_INSUFFICIENT_SPACE;
	}

	u8 *pBuf = buf;

	*pBuf++ = pCmd->numOfBeacons;
	for(u8 i = 0; i < pCmd->numOfBeacons; i++){
		*pBuf++ = LO_UINT16(pCmd->pBeacon[i].deviceShort);
		*pBuf++ = HI_UINT16(pCmd->pBeacon[i].deviceShort);
		*pBuf++ = pCmd->pBeacon[i].rssi;
		*pBuf++ = pCmd->pBeacon[i].classificationMask;
	}

	status = zcl_sendCmd(srcEp, pDstEpInfo, ZCL_CLUSTER_WWAH, ZCL_CMD_WWAH_SURVEY_BEACONS_RSP, TRUE,
    		ZCL_FRAME_SERVER_CLIENT_DIR, disableDefaultRsp, 0, seqNo, len, buf);

    ev_buf_free(buf);

    return status;
}



_CODE_ZCL_ static u8 zcl_wwah_clustersToExemptPrc(zclIncoming_t *pInMsg)
{
	status_t status = ZCL_STA_SUCCESS;
    u8 *pData = pInMsg->pData;

    if( (pInMsg->hdr.cmd == ZCL_CMD_WWAH_ENABLE_APS_LINK_KEY_AUTH) ||
    	(pInMsg->hdr.cmd == ZCL_CMD_WWAH_DISABLE_APS_LINK_KEY_AUTH) ){
		if(!(pInMsg->msg->indInfo.security_status & SECURITY_IN_APSLAYER)){
			return ZCL_STA_NOT_AUTHORIZED;
		}
    }

	if( (pInMsg->hdr.cmd == ZCL_CMD_WWAH_ENABLE_APS_LINK_KEY_AUTH) ||
		(pInMsg->hdr.cmd == ZCL_CMD_WWAH_DISABLE_APS_LINK_KEY_AUTH)	){
		g_zcl_apsLinkKeyAuthInfo.status = (pInMsg->hdr.cmd == ZCL_CMD_WWAH_ENABLE_APS_LINK_KEY_AUTH) ? APS_LINK_KEY_AUTH_ENABLE
																									 : APS_LINK_KEY_AUTH_DISABLE;
		g_zcl_apsLinkKeyAuthInfo.num = *pData++;
		g_zcl_apsLinkKeyAuthInfo.num = (g_zcl_apsLinkKeyAuthInfo.num > CLUSTER_ID_MAX_NUM) ? CLUSTER_ID_MAX_NUM
																						   : g_zcl_apsLinkKeyAuthInfo.num;
		for(u8 i = 0; i < g_zcl_apsLinkKeyAuthInfo.num; i++){
			g_zcl_apsLinkKeyAuthInfo.clusterID[i] = BUILD_U16(pData[0], pData[1]);
			pData += 2;
		}

		TL_SCHEDULE_TASK(zcl_wwah_apsLinkKeyAuthInfoSave, NULL);
	}else if(pInMsg->hdr.cmd == ZCL_CMD_WWAH_REQUIRE_APS_ACKS_ON_UNICASTS){
		g_zcl_apsAcksRequireInfo.num = *pData++;
		g_zcl_apsAcksRequireInfo.num = (g_zcl_apsAcksRequireInfo.num > CLUSTER_ID_MAX_NUM) ? CLUSTER_ID_MAX_NUM
																						   : g_zcl_apsAcksRequireInfo.num;
		for(u8 i = 0; i < g_zcl_apsAcksRequireInfo.num; i++){
			g_zcl_apsAcksRequireInfo.clusterID[i] = BUILD_U16(pData[0], pData[1]);
			pData += 2;
		}

		TL_SCHEDULE_TASK(zcl_wwah_apsAcksRequireInfoSave, NULL);
	}

	return status;
}

_CODE_ZCL_ static u8 zcl_wwah_apsLinkKeyAuthQueryPrc(zclIncoming_t *pInMsg)
{
	status_t status = ZCL_STA_SUCCESS;
    apsdeDataInd_t *pApsdeInd = (apsdeDataInd_t*)pInMsg->msg;
    u8 *pData = pInMsg->pData;

    if(UNICAST_MSG(pApsdeInd)){
		zcl_wwah_apsLinkKeyAuthQueryRspCmd_t queryRsp;

		queryRsp.clusterID = BUILD_U16(pData[0], pData[1]);
		queryRsp.status = (bool)zcl_wwah_apsLinkKeyAuthSearch(queryRsp.clusterID);

		epInfo_t dstEp;
		TL_SETSTRUCTCONTENT(dstEp, 0);

		dstEp.dstAddrMode = APS_SHORT_DSTADDR_WITHEP;
		dstEp.dstAddr.shortAddr = pApsdeInd->indInfo.src_short_addr;
		dstEp.dstEp = pApsdeInd->indInfo.src_ep;
		dstEp.profileId = pApsdeInd->indInfo.profile_id;
		dstEp.txOptions |= APS_TX_OPT_ACK_TX;
		if(pInMsg->msg->indInfo.security_status & SECURITY_IN_APSLAYER){
			dstEp.txOptions |= APS_TX_OPT_SECURITY_ENABLED;
		}

		status = zcl_wwah_apsLinkKeyAuthQueryRspCmd(pApsdeInd->indInfo.dst_ep, &dstEp, pInMsg->hdr.manufCode, TRUE, pInMsg->hdr.seqNum, &queryRsp);

		if(status == ZCL_STA_SUCCESS){
			status = ZCL_STA_CMD_HAS_RESP;
		}
    }

	return status;
}

_CODE_ZCL_ static u8 zcl_wwah_requestNewApsLinkKeyPrc(zclIncoming_t *pInMsg)
{
	status_t status = ZCL_STA_SUCCESS;
    apsdeDataInd_t *pApsdeInd = (apsdeDataInd_t*)pInMsg->msg;

    if(UNICAST_MSG(pApsdeInd)){
    	TL_SCHEDULE_TASK(zcl_wwah_reqNewApsLinkKey, NULL);
    }

	return status;
}

_CODE_ZCL_ static u8 zcl_wwah_enableAppEvtRetryAlgorithmPrc(zclIncoming_t *pInMsg)
{
	status_t status = ZCL_STA_SUCCESS;
    apsdeDataInd_t *pApsdeInd = (apsdeDataInd_t*)pInMsg->msg;
    u8 *pData = pInMsg->pData;

	if(pInMsg->clusterAppCb){
		zclIncomingAddrInfo_t addrInfo;
		addrInfo.dirCluster = pInMsg->hdr.frmCtrl.bf.dir;
		addrInfo.profileId = pApsdeInd->indInfo.profile_id;
		addrInfo.srcAddr = pApsdeInd->indInfo.src_short_addr;
		addrInfo.dstAddr = pApsdeInd->indInfo.dst_addr;
		addrInfo.srcEp = pApsdeInd->indInfo.src_ep;
		addrInfo.dstEp = pApsdeInd->indInfo.dst_ep;

		zcl_wwah_enableAppEvtRetryAlgorithmCmd_t cmd;
		TL_SETSTRUCTCONTENT(cmd, 0);

		cmd.firstBackoffTimeInSeconds = *pData++;
		cmd.backoffSequenceCommonRatio = *pData++;
		cmd.maxBackoffTimeInSeconds = BUILD_U32(pData[0], pData[1], pData[2], pData[3]);
		pData += 4;
		cmd.maxReDeliveryAttepts = *pData++;

		status = pInMsg->clusterAppCb(&addrInfo, pInMsg->hdr.cmd, &cmd);
	}else{
		status = ZCL_STA_FAILURE;
	}

	return status;
}

_CODE_ZCL_ static u8 zcl_wwah_disableAppEvtRetryAlgorithmPrc(zclIncoming_t *pInMsg)
{
	status_t status = ZCL_STA_SUCCESS;
    apsdeDataInd_t *pApsdeInd = (apsdeDataInd_t*)pInMsg->msg;

	if(pInMsg->clusterAppCb){
		zclIncomingAddrInfo_t addrInfo;
		addrInfo.dirCluster = pInMsg->hdr.frmCtrl.bf.dir;
		addrInfo.profileId = pApsdeInd->indInfo.profile_id;
		addrInfo.srcAddr = pApsdeInd->indInfo.src_short_addr;
		addrInfo.dstAddr = pApsdeInd->indInfo.dst_addr;
		addrInfo.srcEp = pApsdeInd->indInfo.src_ep;
		addrInfo.dstEp = pApsdeInd->indInfo.dst_ep;

		//no payload.

		status = pInMsg->clusterAppCb(&addrInfo, pInMsg->hdr.cmd, NULL);
	}else{
		status = ZCL_STA_FAILURE;
	}

	return status;
}

_CODE_ZCL_ static u8 zcl_wwah_requestTimePrc(zclIncoming_t *pInMsg)
{
	status_t status = ZCL_STA_SUCCESS;
    apsdeDataInd_t *pApsdeInd = (apsdeDataInd_t*)pInMsg->msg;

	if(pInMsg->clusterAppCb){
		zclIncomingAddrInfo_t addrInfo;
		addrInfo.dirCluster = pInMsg->hdr.frmCtrl.bf.dir;
		addrInfo.profileId = pApsdeInd->indInfo.profile_id;
		addrInfo.srcAddr = pApsdeInd->indInfo.src_short_addr;
		addrInfo.dstAddr = pApsdeInd->indInfo.dst_addr;
		addrInfo.srcEp = pApsdeInd->indInfo.src_ep;
		addrInfo.dstEp = pApsdeInd->indInfo.dst_ep;

		//no payload.

		status = pInMsg->clusterAppCb(&addrInfo, pInMsg->hdr.cmd, NULL);
	}else{
		status = ZCL_STA_FAILURE;
	}

	return status;
}

_CODE_ZCL_ static u8 zcl_wwah_enableRejoinAlgorithmPrc(zclIncoming_t *pInMsg)
{
	status_t status = ZCL_STA_SUCCESS;
    apsdeDataInd_t *pApsdeInd = (apsdeDataInd_t*)pInMsg->msg;
    u8 *pData = pInMsg->pData;

	if(pInMsg->clusterAppCb){
		zclIncomingAddrInfo_t addrInfo;
		addrInfo.dirCluster = pInMsg->hdr.frmCtrl.bf.dir;
		addrInfo.profileId = pApsdeInd->indInfo.profile_id;
		addrInfo.srcAddr = pApsdeInd->indInfo.src_short_addr;
		addrInfo.dstAddr = pApsdeInd->indInfo.dst_addr;
		addrInfo.srcEp = pApsdeInd->indInfo.src_ep;
		addrInfo.dstEp = pApsdeInd->indInfo.dst_ep;

		zcl_wwah_enableRejoinAlgorithmCmd_t cmd;
		TL_SETSTRUCTCONTENT(cmd, 0);

		cmd.fastRejoinTimeoutInSeconds = BUILD_U16(pData[0], pData[1]);
		pData += 2;
		cmd.durationBetweenEachRejoinInSeconds = BUILD_U16(pData[0], pData[1]);
		pData += 2;
		cmd.fastRejoinFirstBackoffInSeconds = BUILD_U16(pData[0], pData[1]);
		pData += 2;
		cmd.maxBackoffTimeInSeconds = BUILD_U16(pData[0], pData[1]);
		pData += 2;
		cmd.maxBackoffIterations = BUILD_U16(pData[0], pData[1]);
		pData += 2;

		status = pInMsg->clusterAppCb(&addrInfo, pInMsg->hdr.cmd, &cmd);
	}else{
		status = ZCL_STA_FAILURE;
	}

	return status;
}

_CODE_ZCL_ static u8 zcl_wwah_disableRejoinAlgorithmPrc(zclIncoming_t *pInMsg)
{
	status_t status = ZCL_STA_SUCCESS;
    apsdeDataInd_t *pApsdeInd = (apsdeDataInd_t*)pInMsg->msg;

	if(pInMsg->clusterAppCb){
		zclIncomingAddrInfo_t addrInfo;
		addrInfo.dirCluster = pInMsg->hdr.frmCtrl.bf.dir;
		addrInfo.profileId = pApsdeInd->indInfo.profile_id;
		addrInfo.srcAddr = pApsdeInd->indInfo.src_short_addr;
		addrInfo.dstAddr = pApsdeInd->indInfo.dst_addr;
		addrInfo.srcEp = pApsdeInd->indInfo.src_ep;
		addrInfo.dstEp = pApsdeInd->indInfo.dst_ep;

		//no payload.

		status = pInMsg->clusterAppCb(&addrInfo, pInMsg->hdr.cmd, NULL);
	}else{
		status = ZCL_STA_FAILURE;
	}

	return status;
}

_CODE_ZCL_ static u8 zcl_wwah_setIasZoneEnrollmentMethodPrc(zclIncoming_t *pInMsg)
{
	status_t status = ZCL_STA_SUCCESS;
    apsdeDataInd_t *pApsdeInd = (apsdeDataInd_t*)pInMsg->msg;
    u8 *pData = pInMsg->pData;

	if(pInMsg->clusterAppCb){
		zclIncomingAddrInfo_t addrInfo;
		addrInfo.dirCluster = pInMsg->hdr.frmCtrl.bf.dir;
		addrInfo.profileId = pApsdeInd->indInfo.profile_id;
		addrInfo.srcAddr = pApsdeInd->indInfo.src_short_addr;
		addrInfo.dstAddr = pApsdeInd->indInfo.dst_addr;
		addrInfo.srcEp = pApsdeInd->indInfo.src_ep;
		addrInfo.dstEp = pApsdeInd->indInfo.dst_ep;

		zcl_wwah_setIasZoneEnrollmentMethodCmd_t cmd;
		TL_SETSTRUCTCONTENT(cmd, 0);

		cmd.enrollmentMode = *pData++;

		status = pInMsg->clusterAppCb(&addrInfo, pInMsg->hdr.cmd, &cmd);
	}else{
		status = ZCL_STA_FAILURE;
	}

	return status;
}

_CODE_ZCL_ static u8 zcl_wwah_clearBindingTablePrc(zclIncoming_t *pInMsg)
{
	status_t status = ZCL_STA_SUCCESS;
    apsdeDataInd_t *pApsdeInd = (apsdeDataInd_t*)pInMsg->msg;

	if(!(pInMsg->msg->indInfo.security_status & SECURITY_IN_APSLAYER)){
		return ZCL_STA_NOT_AUTHORIZED;
	}

    if(UNICAST_MSG(pApsdeInd)){
    	aps_bindingTab_clear();
    }

	return status;
}

_CODE_ZCL_ static u8 zcl_wwah_enablePeriodicRouterCheckInsPrc(zclIncoming_t *pInMsg)
{
	status_t status = ZCL_STA_SUCCESS;
    apsdeDataInd_t *pApsdeInd = (apsdeDataInd_t*)pInMsg->msg;
    u8 *pData = pInMsg->pData;

	if(!(pInMsg->msg->indInfo.security_status & SECURITY_IN_APSLAYER)){
		return ZCL_STA_NOT_AUTHORIZED;
	}

    if(UNICAST_MSG(pApsdeInd)){
		u16 checkInInterval = BUILD_U16(pData[0], pData[1]);

		bool enable = TRUE;
    	zcl_setAttrVal(pInMsg->msg->indInfo.dst_ep, ZCL_CLUSTER_WWAH, ZCL_ATTRID_WWAH_ROUTER_CHECK_IN_ENABLED, (u8 *)&enable);

    	if(g_zcl_perRouterCheckInInfo.periodicRouterCheckInEvt){
    		TL_ZB_TIMER_CANCEL(&g_zcl_perRouterCheckInInfo.periodicRouterCheckInEvt);
    	}
    	if(checkInInterval){
    		g_zcl_perRouterCheckInInfo.manufCode = pInMsg->hdr.manufCode;
    		g_zcl_perRouterCheckInInfo.srcEp = pApsdeInd->indInfo.dst_ep;
    		g_zcl_perRouterCheckInInfo.dstEpInfo.dstEp = pApsdeInd->indInfo.src_ep;
    		g_zcl_perRouterCheckInInfo.dstEpInfo.profileId = pApsdeInd->indInfo.profile_id;
    		g_zcl_perRouterCheckInInfo.dstEpInfo.dstAddrMode = APS_SHORT_DSTADDR_WITHEP;
    		g_zcl_perRouterCheckInInfo.dstEpInfo.dstAddr.shortAddr = 0x0000;
    		g_zcl_perRouterCheckInInfo.dstEpInfo.txOptions |= APS_TX_OPT_ACK_TX;
    		if(pApsdeInd->indInfo.security_status & SECURITY_IN_APSLAYER){
    			g_zcl_perRouterCheckInInfo.dstEpInfo.txOptions |= APS_TX_OPT_SECURITY_ENABLED;
    		}

    		g_zcl_perRouterCheckInInfo.periodicRouterCheckInEvt = TL_ZB_TIMER_SCHEDULE(periodicRouterCheckInCb, (void *)((u32)checkInInterval), 1 * 1000 * 1000);
    	}
    }

	return status;
}

_CODE_ZCL_ static u8 zcl_wwah_disablePeriodicRouterCheckInsPrc(zclIncoming_t *pInMsg)
{
	status_t status = ZCL_STA_SUCCESS;
    apsdeDataInd_t *pApsdeInd = (apsdeDataInd_t*)pInMsg->msg;

	if(!(pInMsg->msg->indInfo.security_status & SECURITY_IN_APSLAYER)){
		return ZCL_STA_NOT_AUTHORIZED;
	}

    if(UNICAST_MSG(pApsdeInd)){
		bool enable = FALSE;
    	zcl_setAttrVal(pInMsg->msg->indInfo.dst_ep, ZCL_CLUSTER_WWAH, ZCL_ATTRID_WWAH_ROUTER_CHECK_IN_ENABLED, (u8 *)&enable);

    	if(g_zcl_perRouterCheckInInfo.periodicRouterCheckInEvt){
    		TL_ZB_TIMER_CANCEL(&g_zcl_perRouterCheckInInfo.periodicRouterCheckInEvt);
    	}
    }

	return status;
}

_CODE_ZCL_ static u8 zcl_wwah_setMacPollFailureWaitTimePrc(zclIncoming_t *pInMsg)
{
	status_t status = ZCL_STA_SUCCESS;
    apsdeDataInd_t *pApsdeInd = (apsdeDataInd_t*)pInMsg->msg;
    u8 *pData = pInMsg->pData;

	if(pInMsg->clusterAppCb){
		zclIncomingAddrInfo_t addrInfo;
		addrInfo.dirCluster = pInMsg->hdr.frmCtrl.bf.dir;
		addrInfo.profileId = pApsdeInd->indInfo.profile_id;
		addrInfo.srcAddr = pApsdeInd->indInfo.src_short_addr;
		addrInfo.dstAddr = pApsdeInd->indInfo.dst_addr;
		addrInfo.srcEp = pApsdeInd->indInfo.src_ep;
		addrInfo.dstEp = pApsdeInd->indInfo.dst_ep;

		zcl_wwah_setMacPollFailureWaitTimeCmd_t cmd;
		TL_SETSTRUCTCONTENT(cmd, 0);

		cmd.waitTime = *pData++;

		status = pInMsg->clusterAppCb(&addrInfo, pInMsg->hdr.cmd, &cmd);
	}else{
		status = ZCL_STA_FAILURE;
	}

	return status;
}

_CODE_ZCL_ static u8 zcl_wwah_setPendingNwkUpdatePrc(zclIncoming_t *pInMsg)
{
	status_t status = ZCL_STA_SUCCESS;
    apsdeDataInd_t *pApsdeInd = (apsdeDataInd_t*)pInMsg->msg;
    u8 *pData = pInMsg->pData;

	if(!(pInMsg->msg->indInfo.security_status & SECURITY_IN_APSLAYER)){
		return ZCL_STA_NOT_AUTHORIZED;
	}

	if(UNICAST_MSG(pApsdeInd)){
		zcl_wwah_setPendingNwkUpdateCmd_t cmd;
		TL_SETSTRUCTCONTENT(cmd, 0);

		cmd.channel = *pData++;
		cmd.panID = BUILD_U16(pData[0], pData[1]);

		zcl_setAttrVal(pInMsg->msg->indInfo.dst_ep, ZCL_CLUSTER_WWAH, ZCL_ATTRID_WWAH_PENDING_NWK_UPDATE_CHANNEL, (u8 *)&cmd.channel);
		zcl_setAttrVal(pInMsg->msg->indInfo.dst_ep, ZCL_CLUSTER_WWAH, ZCL_ATTRID_WWAH_PENDING_NWK_UPDATE_PANID, (u8 *)&cmd.panID);

		zdo_af_set_accept_nwk_update_channel(cmd.channel);
		ZB_SET_ACCEPT_UPDATE_PANID(cmd.panID);
	}

	return status;
}

_CODE_ZCL_ static u8 zcl_wwah_removeApsAcksOnUnicastsRequirementPrc(zclIncoming_t *pInMsg)
{
	status_t status = ZCL_STA_SUCCESS;
    apsdeDataInd_t *pApsdeInd = (apsdeDataInd_t*)pInMsg->msg;

    if(UNICAST_MSG(pApsdeInd)){
    	zcl_wwah_apsAcksRequireInfoClear();
    }

	return status;
}

_CODE_ZCL_ static u8 zcl_wwah_apsAckRequirementQueryPrc(zclIncoming_t *pInMsg)
{
	status_t status = ZCL_STA_SUCCESS;
    apsdeDataInd_t *pApsdeInd = (apsdeDataInd_t*)pInMsg->msg;

    if(UNICAST_MSG(pApsdeInd)){
    	clustersToExempt_t clustersToExempt;
    	TL_SETSTRUCTCONTENT(clustersToExempt, 0);

    	clustersToExempt.numOfClustersToExempt = g_zcl_apsAcksRequireInfo.num;
    	clustersToExempt.pClusterID = g_zcl_apsAcksRequireInfo.clusterID;

		epInfo_t dstEp;
		TL_SETSTRUCTCONTENT(dstEp, 0);

		dstEp.dstAddrMode = APS_SHORT_DSTADDR_WITHEP;
		dstEp.dstAddr.shortAddr = pApsdeInd->indInfo.src_short_addr;
		dstEp.dstEp = pApsdeInd->indInfo.src_ep;
		dstEp.profileId = pApsdeInd->indInfo.profile_id;
		dstEp.txOptions |= APS_TX_OPT_ACK_TX;
		if(pInMsg->msg->indInfo.security_status & SECURITY_IN_APSLAYER){
			dstEp.txOptions |= APS_TX_OPT_SECURITY_ENABLED;
		}

    	status = zcl_wwah_apsAckRequirementQueryRspCmd(pApsdeInd->indInfo.dst_ep, &dstEp, pInMsg->hdr.manufCode, TRUE, pInMsg->hdr.seqNum, &clustersToExempt);

		if(status == ZCL_STA_SUCCESS){
			status = ZCL_STA_CMD_HAS_RESP;
		}
    }

	return status;
}

_CODE_ZCL_ static u8 zcl_wwah_debugReportQueryPrc(zclIncoming_t *pInMsg)
{
	status_t status = ZCL_STA_SUCCESS;
    apsdeDataInd_t *pApsdeInd = (apsdeDataInd_t*)pInMsg->msg;
    u8 *pData = pInMsg->pData;

    if(UNICAST_MSG(pApsdeInd)){
    	u8 curDbgReportID;
    	u16 len;

    	if(zcl_getAttrVal(pApsdeInd->indInfo.dst_ep, ZCL_CLUSTER_WWAH, ZCL_ATTRID_WWAH_CURRENT_DEBUG_REPORT_ID, &len, (u8 *)&curDbgReportID) != ZCL_STA_SUCCESS){
    		return ZCL_STA_NOT_FOUND;
    	}

    	u8 debugReportID = *pData++;

    	if((curDbgReportID == 0) || (debugReportID != curDbgReportID)){
    		return ZCL_STA_NOT_FOUND;
    	}

		zcl_wwah_dbgReportQueryRspCmd_t dbgReportQueryRspCmd;
		TL_SETSTRUCTCONTENT(dbgReportQueryRspCmd, 0);

		dbgReportQueryRspCmd.dbgReportID = debugReportID;
		dbgReportQueryRspCmd.reportDataLen = 0;

		epInfo_t dstEp;
		TL_SETSTRUCTCONTENT(dstEp, 0);

		dstEp.dstAddrMode = APS_SHORT_DSTADDR_WITHEP;
		dstEp.dstAddr.shortAddr = pApsdeInd->indInfo.src_short_addr;
		dstEp.dstEp = pApsdeInd->indInfo.src_ep;
		dstEp.profileId = pApsdeInd->indInfo.profile_id;
		dstEp.txOptions |= APS_TX_OPT_ACK_TX;
		if(pInMsg->msg->indInfo.security_status & SECURITY_IN_APSLAYER){
			dstEp.txOptions |= APS_TX_OPT_SECURITY_ENABLED;
		}

		zcl_wwah_debugReportQueryRspCmd(pApsdeInd->indInfo.dst_ep, &dstEp, pInMsg->hdr.manufCode, TRUE, pInMsg->hdr.seqNum, &dbgReportQueryRspCmd);

		if(status == ZCL_STA_SUCCESS){
			status = ZCL_STA_CMD_HAS_RESP;
		}
    }

	return status;
}

_CODE_ZCL_ static u8 zcl_wwah_surveyBeaconsPrc(zclIncoming_t *pInMsg)
{
	status_t status = ZCL_STA_SUCCESS;
    apsdeDataInd_t *pApsdeInd = (apsdeDataInd_t*)pInMsg->msg;
    u8 *pData = pInMsg->pData;

	if(pInMsg->clusterAppCb){
		zclIncomingAddrInfo_t addrInfo;
		addrInfo.dirCluster = pInMsg->hdr.frmCtrl.bf.dir;
		addrInfo.profileId = pApsdeInd->indInfo.profile_id;
		addrInfo.srcAddr = pApsdeInd->indInfo.src_short_addr;
		addrInfo.dstAddr = pApsdeInd->indInfo.dst_addr;
		addrInfo.srcEp = pApsdeInd->indInfo.src_ep;
		addrInfo.dstEp = pApsdeInd->indInfo.dst_ep;

		zcl_wwah_surveyBeaconsCmd_t cmd;
		TL_SETSTRUCTCONTENT(cmd, 0);

		cmd.standardBeacons = *pData++;

		status = pInMsg->clusterAppCb(&addrInfo, pInMsg->hdr.cmd, &cmd);
	}else{
		status = ZCL_STA_FAILURE;
	}

	return status;
}

_CODE_ZCL_ static u8 zcl_wwah_disableOtaDowngradesPrc(zclIncoming_t *pInMsg)
{
	status_t status = ZCL_STA_SUCCESS;
    apsdeDataInd_t *pApsdeInd = (apsdeDataInd_t*)pInMsg->msg;

    if(UNICAST_MSG(pApsdeInd)){
    	bool disable = TRUE;
    	zcl_setAttrVal(pInMsg->msg->indInfo.dst_ep, ZCL_CLUSTER_WWAH, ZCL_ATTRID_WWAH_DISABLE_OTA_DOWNGRADES, (u8 *)&disable);
    }

	return status;
}

_CODE_ZCL_ static u8 zcl_wwah_disableMgmtLeaveWithoutRejoinPrc(zclIncoming_t *pInMsg)
{
	status_t status = ZCL_STA_SUCCESS;
    apsdeDataInd_t *pApsdeInd = (apsdeDataInd_t*)pInMsg->msg;

    if(UNICAST_MSG(pApsdeInd)){
    	bool enable = FALSE;
    	zcl_setAttrVal(pInMsg->msg->indInfo.dst_ep, ZCL_CLUSTER_WWAH, ZCL_ATTRID_WWAH_MGMT_LEAVE_WITHOUT_REJOIN_ENABLED, (u8 *)&enable);

    	zdo_af_set_disable_mgmtLeave_unsecurity(TRUE);
    }

	return status;
}

_CODE_ZCL_ static u8 zcl_wwah_disableTouchlinkInterpanMsgPrc(zclIncoming_t *pInMsg)
{
	status_t status = ZCL_STA_SUCCESS;
    apsdeDataInd_t *pApsdeInd = (apsdeDataInd_t*)pInMsg->msg;

    if(UNICAST_MSG(pApsdeInd)){
    	bool enable = FALSE;
    	zcl_setAttrVal(pInMsg->msg->indInfo.dst_ep, ZCL_CLUSTER_WWAH, ZCL_ATTRID_WWAH_TOUCHLINK_INTERPAN_ENABLED, (u8 *)&enable);
    }

	return status;
}

_CODE_ZCL_ static u8 zcl_wwah_parentClassificationPrc(zclIncoming_t *pInMsg)
{
	status_t status = ZCL_STA_SUCCESS;
    apsdeDataInd_t *pApsdeInd = (apsdeDataInd_t*)pInMsg->msg;

    if(UNICAST_MSG(pApsdeInd)){
    	bool enable = FALSE;

    	if(pInMsg->hdr.cmd == ZCL_CMD_WWAH_ENABLE_PARENT_CLASSIFICATION){
    		enable = TRUE;
    	}else if(pInMsg->hdr.cmd == ZCL_CMD_WWAH_DISABLE_PARENT_CLASSIFICATION){
    		enable = FALSE;
    	}

    	zcl_setAttrVal(pInMsg->msg->indInfo.dst_ep, ZCL_CLUSTER_WWAH, ZCL_ATTRID_WWAH_PARENT_CLASSIFICATION_ENABLED, (u8 *)&enable);
    }

	return status;
}

_CODE_ZCL_ static u8 zcl_wwah_enableTcSecurityOnNwkKeyRotationPrc(zclIncoming_t *pInMsg)
{
	status_t status = ZCL_STA_SUCCESS;
    apsdeDataInd_t *pApsdeInd = (apsdeDataInd_t*)pInMsg->msg;

    if(UNICAST_MSG(pApsdeInd)){
    	bool enable = TRUE;
    	zcl_setAttrVal(pInMsg->msg->indInfo.dst_ep, ZCL_CLUSTER_WWAH, ZCL_ATTRID_WWAH_TC_SECURITY_ON_NWK_KEY_ROTATION_ENABLED, (u8 *)&enable);

    	zdo_af_set_enable_tcSecOnNwkKeyRotation(TRUE);
    }

	return status;
}

_CODE_ZCL_ static u8 zcl_wwah_badParentRecoveryPrc(zclIncoming_t *pInMsg)
{
	status_t status = ZCL_STA_SUCCESS;
    apsdeDataInd_t *pApsdeInd = (apsdeDataInd_t*)pInMsg->msg;

	if(pInMsg->clusterAppCb){
		zclIncomingAddrInfo_t addrInfo;
		addrInfo.dirCluster = pInMsg->hdr.frmCtrl.bf.dir;
		addrInfo.profileId = pApsdeInd->indInfo.profile_id;
		addrInfo.srcAddr = pApsdeInd->indInfo.src_short_addr;
		addrInfo.dstAddr = pApsdeInd->indInfo.dst_addr;
		addrInfo.srcEp = pApsdeInd->indInfo.src_ep;
		addrInfo.dstEp = pApsdeInd->indInfo.dst_ep;

		//no payload.

		status = pInMsg->clusterAppCb(&addrInfo, pInMsg->hdr.cmd, NULL);
	}else{
		status = ZCL_STA_FAILURE;
	}

	return status;
}

_CODE_ZCL_ static u8 zcl_wwah_configurationModePrc(zclIncoming_t *pInMsg)
{
	status_t status = ZCL_STA_SUCCESS;
    apsdeDataInd_t *pApsdeInd = (apsdeDataInd_t*)pInMsg->msg;

    if(UNICAST_MSG(pApsdeInd)){
    	bool enable = TRUE;

    	if(pInMsg->hdr.cmd == ZCL_CMD_WWAH_ENABLE_CONFIGURATION_MODE){
    		enable = TRUE;
    	}else if(pInMsg->hdr.cmd == ZCL_CMD_WWAH_DISABLE_CONFIGURATION_MODE){
    		enable = FALSE;
    	}

    	zcl_setAttrVal(pInMsg->msg->indInfo.dst_ep, ZCL_CLUSTER_WWAH, ZCL_ATTRID_WWAH_CONFIGURATION_MODE_ENABLED, (u8 *)&enable);

    	zdo_af_set_disable_configuration_mode(!enable);
    }

	return status;
}

_CODE_ZCL_ static u8 zcl_wwah_useTrustCenterForClusterPrc(zclIncoming_t *pInMsg)
{
	status_t status = ZCL_STA_SUCCESS;
    apsdeDataInd_t *pApsdeInd = (apsdeDataInd_t*)pInMsg->msg;
    u8 *pData = pInMsg->pData;

    if(UNICAST_MSG(pApsdeInd)){
    	wwah_useTCForClusterInfo_t useTCForClusterInfo;
    	TL_SETSTRUCTCONTENT(useTCForClusterInfo, 0);

    	u8 numOfCluster = *pData++;
    	numOfCluster = (numOfCluster > CLUSTER_ID_MAX_NUM) ? CLUSTER_ID_MAX_NUM : numOfCluster;

    	clusterInfo_t *pCluster = NULL;
    	for(u8 i = 0; i < numOfCluster; i++){
    		pCluster = zcl_findCluster(pApsdeInd->indInfo.dst_ep, BUILD_U16(pData[0], pData[1]));
    		if(pCluster){
    			useTCForClusterInfo.clusterID[useTCForClusterInfo.num++] = BUILD_U16(pData[0], pData[1]);
    		}
    		pData += 2;
    	}

    	if(useTCForClusterInfo.num){

    		g_zcl_useTCForClusterInfo.num = useTCForClusterInfo.num;

    		for(u8 i = 0; i < useTCForClusterInfo.num; i++){
    			g_zcl_useTCForClusterInfo.clusterID[i] = useTCForClusterInfo.clusterID[i];

    			if(g_zcl_useTCForClusterInfo.clusterID[i] == ZCL_CLUSTER_OTA){
#ifdef ZCL_OTA
    				ota_wwah_useTrustCenter(pApsdeInd->indInfo.src_ep);
#endif
    			}
    		}

    		TL_SCHEDULE_TASK(zcl_wwah_useTCForClusterInfoSave, NULL);
    	}
    }

	return status;
}

_CODE_ZCL_ static u8 zcl_wwah_trustCenterForClusterServerQueryPrc(zclIncoming_t *pInMsg)
{
	status_t status = ZCL_STA_SUCCESS;
    apsdeDataInd_t *pApsdeInd = (apsdeDataInd_t*)pInMsg->msg;

    if(UNICAST_MSG(pApsdeInd)){
    	zcl_wwah_tcForClusterServerQueryRspCmd_t tcForClusterServerQueryRspCmd;
    	TL_SETSTRUCTCONTENT(tcForClusterServerQueryRspCmd, 0);

    	tcForClusterServerQueryRspCmd.numOfClusters = g_zcl_useTCForClusterInfo.num;
    	tcForClusterServerQueryRspCmd.pClusterID = g_zcl_useTCForClusterInfo.clusterID;

		epInfo_t dstEp;
		TL_SETSTRUCTCONTENT(dstEp, 0);

		dstEp.dstAddrMode = APS_SHORT_DSTADDR_WITHEP;
		dstEp.dstAddr.shortAddr = pApsdeInd->indInfo.src_short_addr;
		dstEp.dstEp = pApsdeInd->indInfo.src_ep;
		dstEp.profileId = pApsdeInd->indInfo.profile_id;
		dstEp.txOptions |= APS_TX_OPT_ACK_TX;
		if(pInMsg->msg->indInfo.security_status & SECURITY_IN_APSLAYER){
			dstEp.txOptions |= APS_TX_OPT_SECURITY_ENABLED;
		}

		status = zcl_wwah_trustCenterForClusterServerQueryRspCmd(pApsdeInd->indInfo.dst_ep, &dstEp, pInMsg->hdr.manufCode, TRUE, pInMsg->hdr.seqNum, &tcForClusterServerQueryRspCmd);

		if(status == ZCL_STA_SUCCESS){
			status = ZCL_STA_CMD_HAS_RESP;
		}
    }

	return status;
}

_CODE_ZCL_ static u8 zcl_wwah_apsLinkKeyAuthQueryRspPrc(zclIncoming_t *pInMsg)
{
	status_t status = ZCL_STA_SUCCESS;
    apsdeDataInd_t *pApsdeInd = (apsdeDataInd_t*)pInMsg->msg;
    u8 *pData = pInMsg->pData;

	if(pInMsg->clusterAppCb){
		zclIncomingAddrInfo_t addrInfo;
		addrInfo.dirCluster = pInMsg->hdr.frmCtrl.bf.dir;
		addrInfo.profileId = pApsdeInd->indInfo.profile_id;
		addrInfo.srcAddr = pApsdeInd->indInfo.src_short_addr;
		addrInfo.dstAddr = pApsdeInd->indInfo.dst_addr;
		addrInfo.srcEp = pApsdeInd->indInfo.src_ep;
		addrInfo.dstEp = pApsdeInd->indInfo.dst_ep;

		zcl_wwah_apsLinkKeyAuthQueryRspCmd_t cmd;
		TL_SETSTRUCTCONTENT(cmd, 0);

		cmd.clusterID = BUILD_U16(pData[0], pData[1]);
		pData += 2;
		cmd.status = *pData++;

		status = pInMsg->clusterAppCb(&addrInfo, pInMsg->hdr.cmd, &cmd);
	}else{
		status = ZCL_STA_FAILURE;
	}

	return status;
}

_CODE_ZCL_ static u8 zcl_wwah_poweringNotificationPrc(zclIncoming_t *pInMsg)
{
	status_t status = ZCL_STA_SUCCESS;
    apsdeDataInd_t *pApsdeInd = (apsdeDataInd_t*)pInMsg->msg;
    u8 *pData = pInMsg->pData;

	if(pInMsg->clusterAppCb){
		zclIncomingAddrInfo_t addrInfo;
		addrInfo.dirCluster = pInMsg->hdr.frmCtrl.bf.dir;
		addrInfo.profileId = pApsdeInd->indInfo.profile_id;
		addrInfo.srcAddr = pApsdeInd->indInfo.src_short_addr;
		addrInfo.dstAddr = pApsdeInd->indInfo.dst_addr;
		addrInfo.srcEp = pApsdeInd->indInfo.src_ep;
		addrInfo.dstEp = pApsdeInd->indInfo.dst_ep;

		zcl_wwah_poweringNotificationCmd_t cmd;
		TL_SETSTRUCTCONTENT(cmd, 0);

		cmd.powerNotiReason = *pData++;
		cmd.manuID = BUILD_U16(pData[0], pData[1]);
		pData += 2;
		cmd.manuPowerNotiReasonLen = *pData++;
		cmd.pManuPowerNotiReason = cmd.manuPowerNotiReasonLen ? pData : NULL;

		status = pInMsg->clusterAppCb(&addrInfo, pInMsg->hdr.cmd, &cmd);
	}else{
		status = ZCL_STA_FAILURE;
	}

	return status;
}

_CODE_ZCL_ static u8 zcl_wwah_shortAddressChangePrc(zclIncoming_t *pInMsg)
{
	status_t status = ZCL_STA_SUCCESS;
    apsdeDataInd_t *pApsdeInd = (apsdeDataInd_t*)pInMsg->msg;
    u8 *pData = pInMsg->pData;

	if(pInMsg->clusterAppCb){
		zclIncomingAddrInfo_t addrInfo;
		addrInfo.dirCluster = pInMsg->hdr.frmCtrl.bf.dir;
		addrInfo.profileId = pApsdeInd->indInfo.profile_id;
		addrInfo.srcAddr = pApsdeInd->indInfo.src_short_addr;
		addrInfo.dstAddr = pApsdeInd->indInfo.dst_addr;
		addrInfo.srcEp = pApsdeInd->indInfo.src_ep;
		addrInfo.dstEp = pApsdeInd->indInfo.dst_ep;

		zcl_wwah_shortAddrChangeCmd_t cmd;
		TL_SETSTRUCTCONTENT(cmd, 0);

		ZB_64BIT_ADDR_COPY(cmd.deviceEUI64, pData);
		pData += EXT_ADDR_LEN;
		cmd.deviceShort = BUILD_U16(pData[0], pData[1]);
		pData += 2;

		status = pInMsg->clusterAppCb(&addrInfo, pInMsg->hdr.cmd, &cmd);
	}else{
		status = ZCL_STA_FAILURE;
	}

	return status;
}

_CODE_ZCL_ static u8 zcl_wwah_apsAckRequirementQueryRspPrc(zclIncoming_t *pInMsg)
{
	status_t status = ZCL_STA_SUCCESS;
    apsdeDataInd_t *pApsdeInd = (apsdeDataInd_t*)pInMsg->msg;
    u8 *pData = pInMsg->pData;

	if(pInMsg->clusterAppCb){
		zclIncomingAddrInfo_t addrInfo;
		addrInfo.dirCluster = pInMsg->hdr.frmCtrl.bf.dir;
		addrInfo.profileId = pApsdeInd->indInfo.profile_id;
		addrInfo.srcAddr = pApsdeInd->indInfo.src_short_addr;
		addrInfo.dstAddr = pApsdeInd->indInfo.dst_addr;
		addrInfo.srcEp = pApsdeInd->indInfo.src_ep;
		addrInfo.dstEp = pApsdeInd->indInfo.dst_ep;

		clustersToExempt_t cmd;
		TL_SETSTRUCTCONTENT(cmd, 0);

		cmd.numOfClustersToExempt = *pData++;
		cmd.pClusterID = cmd.numOfClustersToExempt ? (u16 *)pData : NULL;

		status = pInMsg->clusterAppCb(&addrInfo, pInMsg->hdr.cmd, &cmd);
	}else{
		status = ZCL_STA_FAILURE;
	}

	return status;
}

_CODE_ZCL_ static u8 zcl_wwah_powerDescriptorChangePrc(zclIncoming_t *pInMsg)
{
	status_t status = ZCL_STA_SUCCESS;
    apsdeDataInd_t *pApsdeInd = (apsdeDataInd_t*)pInMsg->msg;
    u8 *pData = pInMsg->pData;

	if(pInMsg->clusterAppCb){
		zclIncomingAddrInfo_t addrInfo;
		addrInfo.dirCluster = pInMsg->hdr.frmCtrl.bf.dir;
		addrInfo.profileId = pApsdeInd->indInfo.profile_id;
		addrInfo.srcAddr = pApsdeInd->indInfo.src_short_addr;
		addrInfo.dstAddr = pApsdeInd->indInfo.dst_addr;
		addrInfo.srcEp = pApsdeInd->indInfo.src_ep;
		addrInfo.dstEp = pApsdeInd->indInfo.dst_ep;

		zcl_wwah_powerDescChangeCmd_t cmd;
		TL_SETSTRUCTCONTENT(cmd, 0);

		cmd.currentPowerMode = (pData[0] & 0xf0) >> 4;
		cmd.availablePowerSrcs = pData[0] & 0x0f;
		cmd.currentPowerSrc = (pData[1] & 0xf0) >> 4;
		cmd.currentPowerSrcLevel = pData[1] & 0x0f;

		status = pInMsg->clusterAppCb(&addrInfo, pInMsg->hdr.cmd, &cmd);
	}else{
		status = ZCL_STA_FAILURE;
	}

	return status;
}

_CODE_ZCL_ static u8 zcl_wwah_newDebugReportNotificationPrc(zclIncoming_t *pInMsg)
{
	status_t status = ZCL_STA_SUCCESS;
    apsdeDataInd_t *pApsdeInd = (apsdeDataInd_t*)pInMsg->msg;
    u8 *pData = pInMsg->pData;

	if(pInMsg->clusterAppCb){
		zclIncomingAddrInfo_t addrInfo;
		addrInfo.dirCluster = pInMsg->hdr.frmCtrl.bf.dir;
		addrInfo.profileId = pApsdeInd->indInfo.profile_id;
		addrInfo.srcAddr = pApsdeInd->indInfo.src_short_addr;
		addrInfo.dstAddr = pApsdeInd->indInfo.dst_addr;
		addrInfo.srcEp = pApsdeInd->indInfo.src_ep;
		addrInfo.dstEp = pApsdeInd->indInfo.dst_ep;

		zcl_wwah_newDbgReportNotiCmd_t cmd;
		TL_SETSTRUCTCONTENT(cmd, 0);

		cmd.dbgReportID = *pData++;
		cmd.sizeOfReport = BUILD_U32(pData[0], pData[1], pData[2], pData[3]);
		pData += 4;

		status = pInMsg->clusterAppCb(&addrInfo, pInMsg->hdr.cmd, &cmd);
	}else{
		status = ZCL_STA_FAILURE;
	}

	return status;
}

_CODE_ZCL_ static u8 zcl_wwah_debugReportQueryRspPrc(zclIncoming_t *pInMsg)
{
	status_t status = ZCL_STA_SUCCESS;
    apsdeDataInd_t *pApsdeInd = (apsdeDataInd_t*)pInMsg->msg;
    u8 *pData = pInMsg->pData;

	if(pInMsg->clusterAppCb){
		zclIncomingAddrInfo_t addrInfo;
		addrInfo.dirCluster = pInMsg->hdr.frmCtrl.bf.dir;
		addrInfo.profileId = pApsdeInd->indInfo.profile_id;
		addrInfo.srcAddr = pApsdeInd->indInfo.src_short_addr;
		addrInfo.dstAddr = pApsdeInd->indInfo.dst_addr;
		addrInfo.srcEp = pApsdeInd->indInfo.src_ep;
		addrInfo.dstEp = pApsdeInd->indInfo.dst_ep;

		zcl_wwah_dbgReportQueryRspCmd_t cmd;
		TL_SETSTRUCTCONTENT(cmd, 0);

		cmd.dbgReportID = *pData++;
		cmd.reportDataLen = *pData++;
		cmd.pReportData = cmd.reportDataLen ? pData : NULL;

		status = pInMsg->clusterAppCb(&addrInfo, pInMsg->hdr.cmd, &cmd);
	}else{
		status = ZCL_STA_FAILURE;
	}

	return status;
}

_CODE_ZCL_ static u8 zcl_wwah_trustCenterForClusterServerQueryRspPrc(zclIncoming_t *pInMsg)
{
	status_t status = ZCL_STA_SUCCESS;
    apsdeDataInd_t *pApsdeInd = (apsdeDataInd_t*)pInMsg->msg;
    u8 *pData = pInMsg->pData;

	if(pInMsg->clusterAppCb){
		zclIncomingAddrInfo_t addrInfo;
		addrInfo.dirCluster = pInMsg->hdr.frmCtrl.bf.dir;
		addrInfo.profileId = pApsdeInd->indInfo.profile_id;
		addrInfo.srcAddr = pApsdeInd->indInfo.src_short_addr;
		addrInfo.dstAddr = pApsdeInd->indInfo.dst_addr;
		addrInfo.srcEp = pApsdeInd->indInfo.src_ep;
		addrInfo.dstEp = pApsdeInd->indInfo.dst_ep;

		zcl_wwah_tcForClusterServerQueryRspCmd_t cmd;
		TL_SETSTRUCTCONTENT(cmd, 0);

		cmd.numOfClusters = *pData++;
		cmd.pClusterID = cmd.numOfClusters ? (u16 *)pData : NULL;

		status = pInMsg->clusterAppCb(&addrInfo, pInMsg->hdr.cmd, &cmd);
	}else{
		status = ZCL_STA_FAILURE;
	}

	return status;
}

_CODE_ZCL_ static u8 zcl_wwah_surveyBeaconsRspPrc(zclIncoming_t *pInMsg)
{
	status_t status = ZCL_STA_SUCCESS;
    apsdeDataInd_t *pApsdeInd = (apsdeDataInd_t*)pInMsg->msg;
    u8 *pData = pInMsg->pData;

	if(pInMsg->clusterAppCb){
		zclIncomingAddrInfo_t addrInfo;
		addrInfo.dirCluster = pInMsg->hdr.frmCtrl.bf.dir;
		addrInfo.profileId = pApsdeInd->indInfo.profile_id;
		addrInfo.srcAddr = pApsdeInd->indInfo.src_short_addr;
		addrInfo.dstAddr = pApsdeInd->indInfo.dst_addr;
		addrInfo.srcEp = pApsdeInd->indInfo.src_ep;
		addrInfo.dstEp = pApsdeInd->indInfo.dst_ep;

		zcl_wwah_surveyBeaconsRspCmd_t cmd;
		TL_SETSTRUCTCONTENT(cmd, 0);

		cmd.numOfBeacons = *pData++;
		cmd.pBeacon = cmd.numOfBeacons ? (beaconSurvey_t *)pData : NULL;

		status = pInMsg->clusterAppCb(&addrInfo, pInMsg->hdr.cmd, &cmd);
	}else{
		status = ZCL_STA_FAILURE;
	}

	return status;
}

_CODE_ZCL_ static u8 zcl_wwah_clientCmdHandler(zclIncoming_t *pInMsg)
{
    u8 status = ZCL_STA_SUCCESS;

    switch(pInMsg->hdr.cmd)
    {
		case ZCL_CMD_WWAH_ENABLE_APS_LINK_KEY_AUTH:
		case ZCL_CMD_WWAH_DISABLE_APS_LINK_KEY_AUTH:
		case ZCL_CMD_WWAH_REQUIRE_APS_ACKS_ON_UNICASTS:
			status = zcl_wwah_clustersToExemptPrc(pInMsg);
			break;
		case ZCL_CMD_WWAH_APS_LINK_KEY_AUTH_QUERY:
			status = zcl_wwah_apsLinkKeyAuthQueryPrc(pInMsg);
			break;
		case ZCL_CMD_WWAH_REQUEST_NEW_APS_LINK_KEY:
			status = zcl_wwah_requestNewApsLinkKeyPrc(pInMsg);
			break;
		case ZCL_CMD_WWAH_ENABLE_APP_EVT_RETRY_ALGORITHM:
			status = zcl_wwah_enableAppEvtRetryAlgorithmPrc(pInMsg);
			break;
		case ZCL_CMD_WWAH_DISABLE_APP_EVT_RETRY_ALGORITHM:
			status = zcl_wwah_disableAppEvtRetryAlgorithmPrc(pInMsg);
			break;
		case ZCL_CMD_WWAH_REQUEST_TIME:
			status = zcl_wwah_requestTimePrc(pInMsg);
			break;
		case ZCL_CMD_WWAH_ENABLE_REJOIN_ALGORITHM:
			status = zcl_wwah_enableRejoinAlgorithmPrc(pInMsg);
			break;
		case ZCL_CMD_WWAH_DISABLE_REJOIN_ALGORITHM:
			status = zcl_wwah_disableRejoinAlgorithmPrc(pInMsg);
			break;
		case ZCL_CMD_WWAH_SET_IAS_ZONE_ENROLLMENT_METHOD:
			status = zcl_wwah_setIasZoneEnrollmentMethodPrc(pInMsg);
			break;
		case ZCL_CMD_WWAH_CLEAR_BINDING_TABLE:
			status = zcl_wwah_clearBindingTablePrc(pInMsg);
			break;
		case ZCL_CMD_WWAH_ENABLE_PERIODIC_ROUTER_CHECK_INS:
			status = zcl_wwah_enablePeriodicRouterCheckInsPrc(pInMsg);
			break;
		case ZCL_CMD_WWAH_DISABLE_PERIODIC_ROUTER_CHECK_INS:
			status = zcl_wwah_disablePeriodicRouterCheckInsPrc(pInMsg);
			break;
		case ZCL_CMD_WWAH_SET_MAC_POLL_FAILURE_WAIT_TIME:
			status = zcl_wwah_setMacPollFailureWaitTimePrc(pInMsg);
			break;
		case ZCL_CMD_WWAH_SET_PENDING_NWK_UPDATE:
			status = zcl_wwah_setPendingNwkUpdatePrc(pInMsg);
			break;
		case ZCL_CMD_WWAH_REMOVE_APS_ACKS_ON_UNICASTS_REQUIREMENT:
			status = zcl_wwah_removeApsAcksOnUnicastsRequirementPrc(pInMsg);
			break;
		case ZCL_CMD_WWAH_APS_ACK_REQUIREMENT_QUERY:
			status = zcl_wwah_apsAckRequirementQueryPrc(pInMsg);
			break;
		case ZCL_CMD_WWAH_DEBUG_REPORT_QUERY:
			status = zcl_wwah_debugReportQueryPrc(pInMsg);
			break;
		case ZCL_CMD_WWAH_SURVEY_BEACONS:
			status = zcl_wwah_surveyBeaconsPrc(pInMsg);
			break;
		case ZCL_CMD_WWAH_DISABLE_OTA_DOWNGRADES:
			status = zcl_wwah_disableOtaDowngradesPrc(pInMsg);
			break;
		case ZCL_CMD_WWAH_DISABLE_MGMT_LEAVE_WITHOUT_REJOIN:
			status = zcl_wwah_disableMgmtLeaveWithoutRejoinPrc(pInMsg);
			break;
		case ZCL_CMD_WWAH_DISABLE_TOUCHLINK_INTERPAN_MSG_SUPPORT:
			status = zcl_wwah_disableTouchlinkInterpanMsgPrc(pInMsg);
			break;
		case ZCL_CMD_WWAH_ENABLE_PARENT_CLASSIFICATION:
		case ZCL_CMD_WWAH_DISABLE_PARENT_CLASSIFICATION:
			status = zcl_wwah_parentClassificationPrc(pInMsg);
			break;
		case ZCL_CMD_WWAH_ENABLE_TC_SECURITY_ON_NWK_KEY_ROTATION:
			status = zcl_wwah_enableTcSecurityOnNwkKeyRotationPrc(pInMsg);
			break;
		case ZCL_CMD_WWAH_ENABLE_BAD_PARENT_RECOVERY:
		case ZCL_CMD_WWAH_DISABLE_BAD_PARENT_RECOVERY:
			status = zcl_wwah_badParentRecoveryPrc(pInMsg);
			break;
		case ZCL_CMD_WWAH_ENABLE_CONFIGURATION_MODE:
		case ZCL_CMD_WWAH_DISABLE_CONFIGURATION_MODE:
			status = zcl_wwah_configurationModePrc(pInMsg);
			break;
		case ZCL_CMD_WWAH_USE_TRUST_CENTER_FOR_CLUSTER:
			status = zcl_wwah_useTrustCenterForClusterPrc(pInMsg);
			break;
		case ZCL_CMD_WWAH_TRUST_CENTER_FOR_CLUSTER_SERVER_QUERY:
			status = zcl_wwah_trustCenterForClusterServerQueryPrc(pInMsg);
			break;
		default:
			status = ZCL_STA_UNSUP_CLUSTER_COMMAND;
			break;
    }

    return status;
}

_CODE_ZCL_ static u8 zcl_wwah_serverCmdHandler(zclIncoming_t *pInMsg)
{
    u8 status = ZCL_STA_SUCCESS;

    switch(pInMsg->hdr.cmd)
    {
		case ZCL_CMD_WWAH_APS_LINK_KEY_AUTH_QUERY_RSP:
			status = zcl_wwah_apsLinkKeyAuthQueryRspPrc(pInMsg);
			break;
		case ZCL_CMD_WWAH_POWERING_OFF_NOTIFICATION:
		case ZCL_CMD_WWAH_POWERING_ON_NOTIFICATION:
			status = zcl_wwah_poweringNotificationPrc(pInMsg);
			break;
		case ZCL_CMD_WWAH_SHORT_ADDRESS_CHANGE:
			status = zcl_wwah_shortAddressChangePrc(pInMsg);
			break;
		case ZCL_CMD_WWAH_APS_ACK_REQUIREMENT_QUERY_RSP:
			status = zcl_wwah_apsAckRequirementQueryRspPrc(pInMsg);
			break;
		case ZCL_CMD_WWAH_POWER_DESCRIPTOR_CHANGE:
			status = zcl_wwah_powerDescriptorChangePrc(pInMsg);
			break;
		case ZCL_CMD_WWAH_NEW_DEBUG_REPORT_NOTIFICATION:
			status = zcl_wwah_newDebugReportNotificationPrc(pInMsg);
			break;
		case ZCL_CMD_WWAH_DEBUG_REPORT_QUERY_RSP:
			status = zcl_wwah_debugReportQueryRspPrc(pInMsg);
			break;
		case ZCL_CMD_WWAH_TRUST_CENTER_FOR_CLUSTER_SERVER_QUERY_RSP:
			status = zcl_wwah_trustCenterForClusterServerQueryRspPrc(pInMsg);
			break;
		case ZCL_CMD_WWAH_SURVEY_BEACONS_RSP:
			status = zcl_wwah_surveyBeaconsRspPrc(pInMsg);
			break;
		default:
			status = ZCL_STA_UNSUP_CLUSTER_COMMAND;
			break;
    }

    return status;
}

_CODE_ZCL_ static status_t zcl_wwah_cmdHandler(zclIncoming_t *pInMsg)
{
	if (pInMsg->hdr.frmCtrl.bf.dir == ZCL_FRAME_CLIENT_SERVER_DIR) {
		return zcl_wwah_clientCmdHandler(pInMsg);
	} else {
		return zcl_wwah_serverCmdHandler(pInMsg);
	}
}

#endif /* ZCL_WWAH */
