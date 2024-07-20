#include <kamek.hpp>

#ifndef _WIILINK_TYPES_
#  define _WIILINK_TYPES_

#  define PROD

#define SHA256_DIGEST_SIZE 32

#  ifndef WWFC_DOMAIN

#    ifdef PROD
#      define WWFC_DOMAIN "zplwii.xyz"
#    else
#      define WWFC_DOMAIN "zplwii.xyz" // Points to localhost
#    endif

#  endif

#  define PAYLOAD_BLOCK_SIZE 0x20000

extern "C" u32 WWFC_CUSTOM_REGION; // 0x80005EFC

typedef struct {
    char magic[0xC]; // Always "WWFC/Payload"
    u32 total_size;
    u8 signature[0x100]; // RSA-2048 signature
} __attribute__((packed)) wwfc_payload_header;

typedef struct {
    u32 format_version; // Payload format version
    u32 format_version_compat; // Minimum payload format version that this
                               // payload is compatible with
    char name[0xC]; // Payload name (e.g. "RMCPD00")
    u32 version; // Payload version
    u32 got_start;
    u32 got_end;
    u32 fixup_start;
    u32 fixup_end;
    u32 patch_list_offset;
    u32 patch_list_end;
    u32 entry_point;
    u32 entry_point_no_got;
    u32 reserved[0x18 / 4];
    char build_timestamp[0x20];
} __attribute__((packed)) wwfc_payload_info;

typedef struct {
    wwfc_payload_header header;
    u8 salt[SHA256_DIGEST_SIZE];
    wwfc_payload_info info;
} __attribute__((packed)) wwfc_payload;

typedef enum {
    /**
     * Copy bytes specified in `args` to the destination `address`.
     * @param arg0 Pointer to the data to copy from.
     * @param arg1 Length of the data.
     */
    WWFC_PATCH_TYPE_WRITE = 0,

    /**
     * Write a branch: `address` = b `arg0`;
     * @param arg0 Branch destination address.
     * @param arg1 Not used.
     */
    WWFC_PATCH_TYPE_BRANCH = 1,

    /**
     * Write a branch with a branch back: `address` = b `arg0`; `arg1` = b
     * `address` + 4;
     * @param arg0 Branch destination address.
     * @param arg1 Address to write the branch back.
     */
    WWFC_PATCH_TYPE_BRANCH_HOOK = 2,

    /**
     * Write a branch with link: `address` = bl `arg0`
     * @param arg0 Branch destination address.
     * @param arg1 Not used.
     */
    WWFC_PATCH_TYPE_CALL = 3,

    /**
     * Write a branch using the count register:
     * `address` = \
     * lis `arg1`, `arg0`\@h; \
     * ori `arg1`, `arg1`, `arg0`\@l; \
     * mtctr `arg1`; \
     * bctr;
     * @param arg0 Branch destination address.
     * @param arg1 Temporary register to use for call.
     */
    WWFC_PATCH_TYPE_BRANCH_CTR = 4,

    /**
     * Write a branch with link using the count register:
     * `address` = \
     * lis `arg1`, `arg0`\@h; \
     * ori `arg1`, `arg1`, `arg0`\@l; \
     * mtctr `arg1`; \
     * bctrl;
     * @param arg0 Branch destination address.
     * @param arg1 Temporary register to use for call.
     */
    WWFC_PATCH_TYPE_BRANCH_CTR_LINK = 5,
} wwfc_patch_type;

/**
 * Flags for different patch levels.
 */
typedef enum {

    /**
     * Critical, used for security patches and other things required to connect
     * to the server. This has no value and is always automatically applied.
     */
    WWFC_PATCH_LEVEL_CRITICAL = 0, // 0x00

    /**
     * Patches that fix bugs in the game, such as anti-freeze patches.
     */
    WWFC_PATCH_LEVEL_BUGFIX = 1 << 0, // 0x01

    /**
     * Patches required for parity with clients using a regular WWFC patcher.
     */
    WWFC_PATCH_LEVEL_PARITY = 1 << 1, // 0x02

    /**
     * Additional feature, not required to be compatible with regular clients.
     */
    WWFC_PATCH_LEVEL_FEATURE = 1 << 2, // 0x04

    /**
     * General support patches that may be redundant depending on the patcher.
     * Used in cases like URL patches.
     */
    WWFC_PATCH_LEVEL_SUPPORT = 1 << 3, // 0x08

    /**
     * Flag used to disable the patch if it has been already applied.
     */
    WWFC_PATCH_LEVEL_DISABLED = 1 << 4, // 0x10
} wwfc_patch_level;

typedef struct {
    u8 level; // wwfc_patch_level
    u8 type; // wwfc_patch_type
    u8 reserved[2];
    u32 address;

    union {
        u32 arg0;
        const void* arg0p;
        const u32* arg0p32;
    };

    u32 arg1;
} __attribute__((packed)) wwfc_patch;

#  define WL_ERROR_PAYLOAD_OK 0
#  define WL_ERROR_GECKO -20900
#  define WL_ERROR_PAYLOAD_STAGE0_MISSING_STAGE1 -20901
#  define WL_ERROR_PAYLOAD_STAGE0_HASH_MISMATCH -20902
#  define WL_ERROR_PAYLOAD_STAGE1_ALLOC -20910
#  define WL_ERROR_PAYLOAD_STAGE1_MAKE_REQUEST -20911
#  define WL_ERROR_PAYLOAD_STAGE1_RESPONSE -20912
#  define WL_ERROR_PAYLOAD_STAGE1_HEADER_CHECK -20913
#  define WL_ERROR_PAYLOAD_STAGE1_LENGTH_ERROR -20914
#  define WL_ERROR_PAYLOAD_STAGE1_SALT_MISMATCH -20915
#  define WL_ERROR_PAYLOAD_STAGE1_GAME_ID_MISMATCH -20916
#  define WL_ERROR_PAYLOAD_STAGE1_SIGNATURE_INVALID -20917
#  define WL_ERROR_PAYLOAD_STAGE1_WAITING -20918
#  define WL_ERROR_PAYLOAD_GAME_MISMATCH -20930

#  ifdef PROD

// Production payload key
static const unsigned char PayloadPublicKey[] = {
    0xb9, 0x2e, 0xf3, 0xa7, 0xe3, 0x95, 0x7b, 0xe9, 0x00, 0x3f, 0x88, 0x50,
    0xe1, 0x75, 0xd0, 0x0d, 0x92, 0x1e, 0xd2, 0x8c, 0xe8, 0x84, 0x5b, 0x76,
    0x72, 0x70, 0x19, 0xaf, 0x0a, 0xbc, 0x90, 0x99, 0x86, 0x78, 0xda, 0x22,
    0xf8, 0x66, 0xc9, 0x25, 0x10, 0x43, 0xa2, 0xe2, 0x1b, 0x4a, 0xc7, 0x42,
    0x56, 0x6c, 0x65, 0x3a, 0x43, 0x4b, 0xd3, 0xe2, 0xd2, 0x35, 0x1d, 0xa9,
    0x71, 0x84, 0x67, 0xf4, 0x44, 0xe7, 0x6a, 0x99, 0x7a, 0x13, 0xd4, 0x3c,
    0xad, 0x7a, 0x77, 0x2e, 0x0b, 0xf7, 0xa8, 0x13, 0x7c, 0xea, 0x1f, 0xa2,
    0x3a, 0x20, 0xaf, 0x01, 0xcf, 0x91, 0xbb, 0x8d, 0xa2, 0xc5, 0x0b, 0x39,
    0xac, 0xd0, 0x1f, 0x8e, 0x1b, 0x5f, 0x12, 0x2e, 0xe7, 0x30, 0xa7, 0x6f,
    0xcb, 0x87, 0x40, 0x72, 0x9e, 0x23, 0x33, 0xab, 0xe1, 0x7a, 0xf6, 0xff,
    0xbf, 0x40, 0x4b, 0x70, 0x47, 0x12, 0xc7, 0xe7, 0xfb, 0xa1, 0xad, 0x7f,
    0x9b, 0x13, 0x30, 0xef, 0x06, 0x8c, 0xa1, 0x99, 0xfc, 0x9f, 0xff, 0x95,
    0xb2, 0x2f, 0xc6, 0x8b, 0xb1, 0x6d, 0x77, 0x3e, 0x19, 0xd8, 0xfb, 0x18,
    0x09, 0xdb, 0x2f, 0xd0, 0xb8, 0x48, 0x3a, 0xea, 0x1d, 0x29, 0xee, 0x55,
    0x50, 0xc3, 0xc8, 0xe2, 0xe2, 0x32, 0x25, 0xd3, 0xe4, 0x73, 0x54, 0xd2,
    0x9e, 0x54, 0x46, 0x95, 0xc7, 0x1a, 0xd3, 0xa5, 0x19, 0x69, 0xc0, 0x8e,
    0x0a, 0x47, 0xa4, 0xdf, 0xee, 0x33, 0xf8, 0x92, 0x16, 0x5b, 0x62, 0x4a,
    0xd4, 0xa6, 0x99, 0xed, 0xaa, 0x7d, 0xeb, 0xf3, 0x40, 0xa7, 0x09, 0xa5,
    0x4a, 0xb8, 0xfe, 0x0b, 0x1f, 0x6c, 0x86, 0x1b, 0x57, 0x29, 0x9d, 0xb4,
    0xf9, 0x65, 0x58, 0xd0, 0xcf, 0x26, 0xd5, 0xd5, 0x68, 0x52, 0xb7, 0x68,
    0x24, 0x1a, 0xb1, 0x37, 0x4c, 0x62, 0xe6, 0x6d, 0x52, 0x45, 0xdb, 0xda,
    0x76, 0xa8, 0x10, 0x39, 0xab, 0x11, 0x6f, 0x11, 0xea, 0xa5, 0x4e, 0x59,
    0xa7, 0xd5, 0x7a, 0xe8, 0xb7, 0x8f, 0x1a, 0x37, 0x5e, 0xe7, 0x91, 0xd4,
    0x2a, 0x9c, 0x8a, 0xfc, 0x78, 0xa6, 0x7a, 0x9f, 0x3c, 0xfd, 0x4f, 0x7c,
    0xae, 0x59, 0x49, 0xea, 0x4e, 0x82, 0x35, 0xab, 0x2b, 0xe6, 0xcc, 0x15,
    0xe2, 0xb6, 0xdb, 0xb0, 0x7f, 0xa6, 0x36, 0xc3, 0x75, 0xf0, 0xd9, 0x6e,
    0xb2, 0x31, 0x7d, 0x28, 0xe0, 0x33, 0x34, 0x9a, 0x06, 0x29, 0xef, 0x05,
    0xbf, 0xe9, 0xe6, 0xc3, 0xb1, 0x28, 0x84, 0x58, 0xbb, 0x79, 0x68, 0x4d,
    0x1c, 0x62, 0x38, 0x75, 0x6e, 0xa6, 0xfa, 0xe6, 0x2b, 0x40, 0x81, 0x90,
    0x56, 0xb2, 0x9d, 0x7b, 0x99, 0x29, 0xf5, 0x5a, 0xda, 0x54, 0x6e, 0xa2,
    0x8a, 0x37, 0x25, 0x27, 0x77, 0x67, 0x13, 0xb5, 0x4b, 0xcf, 0x04, 0x64,
    0x02, 0x08, 0x13, 0xde, 0xfd, 0xe7, 0x25, 0x8a, 0xf1, 0x8e, 0xaf, 0x30,
    0xbf, 0xcd, 0x86, 0x3b, 0xe0, 0x3a, 0x94, 0xea, 0xee, 0xc9, 0x3a, 0x58,
    0x8b, 0x1d, 0xe5, 0xaf, 0xed, 0x51, 0x6d, 0x59, 0x63, 0x74, 0x96, 0xce,
    0x46, 0x03, 0xae, 0x3f, 0x40, 0x6c, 0x53, 0x5a, 0xf9, 0xdf, 0x04, 0x85,
    0x4c, 0x32, 0x97, 0x5b, 0x52, 0xa2, 0x97, 0x48, 0x27, 0x09, 0x2f, 0x7a,
    0x19, 0x14, 0x20, 0x2b, 0x68, 0x9d, 0xb7, 0xb0, 0xb7, 0x52, 0xd0, 0x6c,
    0x3b, 0xfd, 0xbd, 0x43, 0xc3, 0xb0, 0xb3, 0xcf, 0xf3, 0x43, 0xf6, 0x2b,
    0xca, 0x9e, 0xf0, 0x54, 0xfb, 0x43, 0x84, 0xec, 0x43, 0xf6, 0x41, 0x60,
    0x28, 0x72, 0xe8, 0x34, 0xf5, 0xd3, 0x6b, 0xee, 0x90, 0xf1, 0x13, 0xe3,
    0xb1, 0x37, 0x5e, 0x73, 0x24, 0x83, 0x8f, 0xf6, 0x06, 0x13, 0xd3, 0xbb,
    0x71, 0xac, 0x24, 0xc8, 0x2c, 0x22, 0x27, 0x76, 0x83, 0x8b, 0xb0, 0xeb,
    0xe0, 0xd2, 0x42, 0x22, 0x0f, 0x30, 0x82, 0xbd, 0x82, 0xb1, 0xb0, 0xe2,
};

#  else

// Development payload key
static const unsigned char PayloadPublicKey[] = {
    0x3f, 0x32, 0xeb, 0x87, 0xb9, 0x75, 0x15, 0xc9, 0x60, 0x17, 0x7c, 0xcf,
    0x52, 0x18, 0x36, 0x66, 0x8e, 0xe9, 0x26, 0xe8, 0x58, 0x1f, 0xff, 0x84,
    0xca, 0x63, 0x63, 0x9f, 0x4f, 0xa5, 0x77, 0x41, 0xe4, 0x4e, 0x5c, 0x49,
    0xb7, 0x54, 0x15, 0xf5, 0x74, 0x70, 0xbc, 0xf4, 0xce, 0x5e, 0x66, 0x78,
    0xf1, 0xe5, 0xfe, 0x3a, 0x98, 0x18, 0xbd, 0x8f, 0x9a, 0xb7, 0x52, 0x99,
    0x7c, 0x80, 0x00, 0x81, 0xf3, 0xc0, 0xce, 0xf1, 0x8b, 0xac, 0x43, 0x18,
    0x44, 0x95, 0x7e, 0xb8, 0x06, 0x41, 0x0e, 0xa7, 0x3e, 0x0e, 0xc0, 0x68,
    0x01, 0x69, 0x88, 0x86, 0x81, 0x2f, 0xfe, 0x5d, 0xfb, 0x62, 0xcf, 0x0e,
    0xdf, 0x4b, 0x1c, 0x67, 0x0f, 0x09, 0xaf, 0x37, 0x40, 0x60, 0x06, 0x34,
    0xe4, 0xc8, 0x98, 0x55, 0xf3, 0xd0, 0xb5, 0xfe, 0x8c, 0x92, 0x92, 0xe0,
    0x69, 0xd9, 0x02, 0x7d, 0xe5, 0x1e, 0x55, 0x00, 0x1c, 0xdf, 0x44, 0xd6,
    0x51, 0x4b, 0xd6, 0xd3, 0xe2, 0xbc, 0xc0, 0xca, 0xf8, 0x42, 0x90, 0xee,
    0x90, 0xf7, 0xc5, 0xce, 0x6d, 0xd2, 0x3d, 0x9e, 0x26, 0x6d, 0x97, 0xe0,
    0xc0, 0x82, 0x8c, 0x63, 0xdc, 0xd6, 0x80, 0x67, 0xc7, 0x58, 0x72, 0xd2,
    0xbe, 0x2f, 0xe4, 0x39, 0x5a, 0x6c, 0x0d, 0x8a, 0xb4, 0x88, 0xef, 0x3d,
    0xd2, 0xde, 0xda, 0x48, 0x62, 0xda, 0x9e, 0x5f, 0xce, 0x3e, 0xd9, 0x09,
    0x1e, 0xc6, 0x74, 0x4d, 0x74, 0xd2, 0x6f, 0xba, 0x83, 0xc0, 0x4b, 0x7e,
    0xe0, 0x76, 0x2c, 0x45, 0xa8, 0x32, 0xac, 0xbc, 0x8d, 0x65, 0xdf, 0xdc,
    0x9d, 0x00, 0x2e, 0x8a, 0xf8, 0x4b, 0x82, 0x35, 0xbd, 0xe3, 0x04, 0x91,
    0xae, 0x3d, 0x5b, 0xe9, 0xef, 0x85, 0xc2, 0xb1, 0xd5, 0xd4, 0xf6, 0x1d,
    0x30, 0xbc, 0xcd, 0x3e, 0x38, 0xf8, 0x51, 0x55, 0xfc, 0x5a, 0x1b, 0x64,
    0x3f, 0x64, 0x37, 0x2e, 0xe5, 0xe7, 0x0d, 0x68, 0x88, 0xfc, 0x55, 0x3d,
    0x69, 0x5e, 0x4d, 0xcd, 0x18, 0xea, 0xd3, 0xd7, 0x41, 0x4c, 0x64, 0x82,
    0x9f, 0x4f, 0xc9, 0x97, 0x0a, 0xe9, 0x74, 0x3c, 0x94, 0xf9, 0x82, 0x7f,
    0x3e, 0x89, 0x9a, 0x4e, 0x62, 0x0e, 0x99, 0xb9, 0x4f, 0xbb, 0xef, 0x05,
    0x18, 0x1f, 0x61, 0xc7, 0xe1, 0xed, 0xf8, 0x70, 0xbf, 0x81, 0xe6, 0x9c,
    0x58, 0xb4, 0x9a, 0xdf, 0x30, 0xdc, 0xbe, 0xb3, 0x38, 0x9f, 0x53, 0x57,
    0x7c, 0xa7, 0x1e, 0xe5, 0xd9, 0x11, 0x67, 0x09, 0x09, 0xc0, 0xc6, 0x20,
    0x69, 0x60, 0xda, 0xd3, 0x1d, 0x87, 0x0e, 0xed, 0x15, 0xf0, 0x85, 0x2c,
    0x68, 0xcd, 0x18, 0xa6, 0x07, 0x04, 0xc1, 0xf9, 0xa4, 0xfb, 0x74, 0x56,
    0x98, 0xd7, 0x46, 0xfc, 0x31, 0x47, 0x6a, 0x09, 0x04, 0xa3, 0x76, 0x3b,
    0x26, 0x83, 0x34, 0xde, 0xd2, 0x22, 0xf2, 0x3f, 0xd4, 0xd9, 0xbb, 0xf0,
    0xba, 0x3f, 0xb3, 0x1e, 0x96, 0x44, 0x04, 0xa1, 0xc8, 0x40, 0xb2, 0x0c,
    0x8a, 0x07, 0xea, 0x16, 0x7d, 0x57, 0x49, 0x9e, 0x6b, 0xec, 0xd3, 0xe1,
    0x77, 0x29, 0x35, 0x6f, 0x82, 0xd7, 0xb8, 0x5a, 0x2b, 0xf3, 0x1a, 0x79,
    0xf9, 0x84, 0xaf, 0x7a, 0x35, 0x85, 0x26, 0xb4, 0xfc, 0x58, 0x2f, 0x89,
    0x2a, 0x97, 0xc8, 0x49, 0xb3, 0x61, 0xbd, 0xf5, 0xda, 0xe9, 0x87, 0x9b,
    0xea, 0x5c, 0x4c, 0x15, 0x19, 0x1f, 0x76, 0x0c, 0x01, 0x6f, 0x32, 0x80,
    0x73, 0xd1, 0xc4, 0x4a, 0x87, 0x32, 0x1e, 0xd6, 0xdf, 0xd9, 0x0a, 0x27,
    0x66, 0xb2, 0x74, 0xc1, 0xca, 0xae, 0x16, 0xca, 0x17, 0x66, 0x09, 0xe9,
    0x37, 0xca, 0x20, 0xfe, 0xa2, 0xa6, 0x7b, 0x2c, 0xd9, 0x6d, 0x91, 0x26,
    0xe6, 0x61, 0xdd, 0xbb, 0x11, 0x18, 0x7c, 0xfb, 0x8f, 0x43, 0x71, 0x6b,
    0x6a, 0xa2, 0x0a, 0xd3, 0xc6, 0x98, 0x94, 0xd8, 0x63, 0xb0, 0x49, 0xf5,
};

#  endif

#endif