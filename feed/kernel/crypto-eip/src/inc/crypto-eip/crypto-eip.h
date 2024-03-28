/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (C) 2023 MediaTek Inc.
 *
 * Author: Chris.Chou <chris.chou@mediatek.com>
 *         Ren-Ting Wang <ren-ting.wang@mediatek.com>
 */

#ifndef _CRYPTO_EIP_H_
#define _CRYPTO_EIP_H_

#include <crypto/sha.h>
#include <linux/io.h>
#include <linux/list.h>
#include <net/xfrm.h>

#include "crypto-eip/crypto-eip197-inline-ddk.h"

struct mtk_crypto;

extern struct mtk_crypto mcrypto;
extern spinlock_t add_lock;

#define TRANSFORM_RECORD_LEN		64

#define MAX_TUNNEL_NUM			10
#define PACKET_INBOUND			1
#define PACKET_OUTBOUND			2

#define HASH_CACHE_SIZE			SHA512_BLOCK_SIZE

#define EIP197_FORCE_CLK_ON2		(0xfffd8)
#define EIP197_FORCE_CLK_ON		(0xfffe8)
#define EIP197_AUTO_LOOKUP_1		(0xfffffffc)
#define EIP197_AUTO_LOOKUP_2		(0xffffffff)

#define PEC_PCL_EIP197
#define CAPWAP_MAX_TUNNEL_NUM CONFIG_TOPS_TNL_NUM

struct mtk_crypto {
	struct mtk_eth *eth;
	void __iomem *crypto_base;
	void __iomem *eth_base;
	u32 ppe_num;
};

struct mtk_xfrm_params {
	struct xfrm_state *xs;
	struct list_head node;
	struct cdrt_entry *cdrt;

	u32 *p_tr;			/* pointer to transform record */
	u32 dir;			/* SABuilder_Direction_t */
};

/* DTLS */
enum dtls_sec_mode_type {
	__DTLS_SEC_MODE_TYPE_NONE = 0,
	AES128_CBC_HMAC_SHA1,
	AES256_CBC_HMAC_SHA1,
	AES128_CBC_HMAC_SHA2_256,
	AES256_CBC_HMAC_SHA2_256,
	AES128_GCM,
	AES256_GCM,
	__DTLS_SEC_MODE_TYPE_MAX = 7,
};

enum dtls_version {
	MTK_DTLS_VERSION_1_0 = 0,
	MTK_DTLS_VERSION_1_2 = 1,
	__DTLS_VERSION_MAX = 2,
};

struct DTLS_param {
	__be32 dip;
	__be32 sip;
	uint16_t dport;
	uint16_t sport;
	uint16_t dtls_epoch;
	uint16_t dtls_version;
	uint8_t sec_mode;
	uint8_t *dtls_encrypt_nonce;
	uint8_t *dtls_decrypt_nonce;
	uint8_t *key_encrypt;
	uint8_t *key_auth_encrypt_1;
	uint8_t *key_auth_encrypt_2;
	uint8_t *key_decrypt;
	uint8_t *key_auth_decrypt_1;
	uint8_t *key_auth_decrypt_2;
	void *SA_encrypt;
	void *SA_decrypt;
} __packed __aligned(16);

struct DTLSResourceMgmt {
	struct DTLS_param *DTLSParam;
	DMABuf_Handle_t DTLSHandleSAOutbound;
	DMABuf_Handle_t DTLSHandleSAInbound;
	uint8_t *HKeyOutbound;
	uint8_t *HKeyInbound;
	uint8_t *InnerDigestOutbound;
	uint8_t *OuterDigestOutbound;
	uint8_t *InnerDigestInbound;
	uint8_t *OuterDigestInbound;
};

struct mtk_cdrt_idx_param {
	uint32_t cdrt_idx_inbound;
	uint32_t cdrt_idx_outbound;
};

struct mtk_CDRT_DTLS_entry {
	struct cdrt_entry *cdrt_inbound;
	struct cdrt_entry *cdrt_outbound;
};

#if defined(CONFIG_MTK_TOPS_CAPWAP_DTLS)
extern void (*mtk_submit_SAparam_to_eip_driver)(struct DTLS_param *DTLSParam_p, int TnlIdx);
extern void (*mtk_remove_SAparam_to_eip_driver)(struct DTLS_param *DTLSParam_p, int TnlIdx);
extern void (*mtk_update_cdrt_idx_from_eip_driver)(struct mtk_cdrt_idx_param *cdrt_idx_params_p);
#endif

void mtk_update_dtls_param(struct DTLS_param *DTLSParam_p, int TnlIdx);
void mtk_remove_dtls_param(struct DTLS_param *DTLSParam_p, int TnlIdx);

/* Netsys */
void crypto_eth_write(u32 reg, u32 val);
u32 mtk_crypto_ppe_get_num(void);

/* xfrm callback functions */
int mtk_xfrm_offload_state_add(struct xfrm_state *xs);
void mtk_xfrm_offload_state_delete(struct xfrm_state *xs);
void mtk_xfrm_offload_state_free(struct xfrm_state *xs);
void mtk_xfrm_offload_state_tear_down(void);
int mtk_xfrm_offload_policy_add(struct xfrm_policy *xp);
bool mtk_xfrm_offload_ok(struct sk_buff *skb, struct xfrm_state *xs);
#endif /* _CRYPTO_EIP_H_ */
