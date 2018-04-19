/*
 * mfalsh.c
 *
 *  Created on: 2018年4月12日
 *      Author: Administrator
 */
#include "mflash.h"
flash_ssd_config_t flashSSDConfig;

/**
 * \brief   写入pflash前的初始化操作
 */
status_t flash_pflash_init(void)
{
    status_t ret;

#ifdef S32K144_SERIES
    /* 禁用缓存 */
    MSCM->OCMDR[0u] |= MSCM_OCMDR_OCM0(0xFu) | MSCM_OCMDR_OCM1(0xFu) | MSCM_OCMDR_OCM2(0xFu);
#endif/* S32K144_SERIES */

    /* 设置中断服务函数 */
    INT_SYS_InstallHandler(FTFC_IRQn, CCIF_Handler, (isr_t*) 0);
    INT_SYS_EnableIRQ(FTFC_IRQn);

    /* 使能全局中断 */
    INT_SYS_EnableIRQGlobal();

    /* 初始化FLASH */
    ret = FLASH_DRV_Init(&flash0_InitConfig0, &flashSSDConfig);
    if (ret != STATUS_SUCCESS)
    {
        return ret;
    }

    /* 设置flash写入完成回掉函数 */
    flashSSDConfig.CallBack = (flash_callback_t)CCIF_Callback;

    return STATUS_SUCCESS;
}

/**
 * \brief   将DFlash初始化为EEPROM
 */
status_t flash_EEPROM_init(void) {
    /* Disable cache to ensure that all flash operations will take effect instantly,
     * this is device dependent */
    status_t ret;

#ifndef FLASH_TARGET
#if (FEATURE_FLS_HAS_PROGRAM_PHRASE_CMD == 1u)
    uint8_t unsecure_key[FTFx_PHRASE_SIZE] = {0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFEu, 0xFFu, 0xFFu, 0xFFu};
#else   /* FEATURE_FLASH_HAS_PROGRAM_LONGWORD_CMD */
    uint8_t unsecure_key[FTFx_LONGWORD_SIZE] = {0xFEu, 0xFFu, 0xFFu, 0xFFu};
#endif  /* FEATURE_FLS_HAS_PROGRAM_PHRASE_CMD */
#endif /* FLASH_TARGET */

#ifdef S32K144_SERIES
    /* 禁用缓存 */
    MSCM->OCMDR[1u] |= MSCM_OCMDR_OCM0(0xFu) | MSCM_OCMDR_OCM1(0xFu) | MSCM_OCMDR_OCM2(0xFu);
    MSCM->OCMDR[2u] |= MSCM_OCMDR_OCM0(0xFu) | MSCM_OCMDR_OCM1(0xFu) | MSCM_OCMDR_OCM2(0xFu);
#endif /* S32K144_SERIES */

    /* 设置中断服务函数 */
    INT_SYS_InstallHandler(FTFC_IRQn, CCIF_Handler, (isr_t*) 0);
    INT_SYS_EnableIRQ(FTFC_IRQn);

    /* 使能全局中断 */
    INT_SYS_EnableIRQGlobal();

    /* 初始化FLASH */
    ret = FLASH_DRV_Init(&flash0_InitConfig0, &flashSSDConfig);
    if (ret != STATUS_SUCCESS)
    {
        return ret;
    }

#if ((FEATURE_FLS_HAS_FLEX_NVM == 1u) & (FEATURE_FLS_HAS_FLEX_RAM == 1u))
    /* 如果flexRam没有作为EEPROM使用,将其设置为EEPROM */
    if (flashSSDConfig.EEESize == 0u)
    {
#ifndef FLASH_TARGET
        /* 擦出扇区 */
        uint32_t address;
        uint32_t size;
        ret = FLASH_DRV_EraseAllBlock(&flashSSDConfig);
        if (ret != STATUS_SUCCESS)
        {
            return ret;
        }

        /* 校验是否擦除成功 */
        ret = FLASH_DRV_VerifyAllBlock(&flashSSDConfig, 1u);
        if (ret != STATUS_SUCCESS)
        {
            return ret;
        }

        /* 安全配置字段Flash编程 */
#if (FEATURE_FLS_HAS_PROGRAM_PHRASE_CMD == 1u)
        address = 0x408u;
        size = FTFx_PHRASE_SIZE;
#else   /* FEATURE_FLASH_HAS_PROGRAM_LONGWORD_CMD == 1u */
        address = 0x40Cu;
        size = FTFx_LONGWORD_SIZE;
#endif /* FEATURE_FLS_HAS_PROGRAM_PHRASE_CMD */
        ret = FLASH_DRV_Program(&flashSSDConfig, address, size, unsecure_key);
        if (ret != STATUS_SUCCESS)
        {
            return ret;
        }
#endif /* FLASH_TARGET */

        /* 将FlexRAM配置为EEPROM,并将FlexNVM作为EEPROM的存储区,
         * 如果IFR区域不是空白将不能成功DEFlashPartition.
         * 关于有效的EEPROM数据大小的代码和代码flexnvm分区设备文件
         * S32K144:
         * - EEEDataSizeCode = 0x02u: EEPROM size = 4 Kbytes
         * - DEPartitionCode = 0x08u: EEPROM backup size = 64 Kbytes
         */
        ret = FLASH_DRV_DEFlashPartition(&flashSSDConfig, 0x02u, 0x08u, 0x0u, false);
        if (ret != STATUS_SUCCESS)
        {
            return ret;
        }
        else
        {
            /* 重新初始化FLASH */
            ret = FLASH_DRV_Init(&flash0_InitConfig0, &flashSSDConfig);
            if (ret != STATUS_SUCCESS)
            {
                return ret;
            }

            /* 使能FlaxRAM为EEPROM */
            ret = FLASH_DRV_SetFlexRamFunction(&flashSSDConfig, EEE_ENABLE, 0x00u, NULL);
            if (ret != STATUS_SUCCESS)
            {
                return ret;
            }
        }
    }
    else    /* FLexRAM已经被初始化为EEPROM */
    {
        /* 使能FlaxRAM为EEPROM */
        ret = FLASH_DRV_SetFlexRamFunction(&flashSSDConfig, EEE_ENABLE, 0x00u, NULL);
        if (ret != STATUS_SUCCESS)
        {
            return ret;
        }
    }
#endif /* (FEATURE_FLS_HAS_FLEX_NVM == 1u) & (FEATURE_FLS_HAS_FLEX_RAM == 1u) */

    /* 设置flash写入完成回掉函数 */
    flashSSDConfig.CallBack = (flash_callback_t)CCIF_Callback;
    return STATUS_SUCCESS;
}

/*
 * \brief   擦出PFlash扇区
 * \param   sector_index    扇区指标,及从第几个扇区开始擦出,
 *                          取值范围0~127(4kByte一个扇区,共512kByte)
 * \param   sector_num      擦出的扇区个数
 * \note    sector_index + sector_num <= 127
 */
status_t flash_pflash_erase_sectors(uint32_t sector_index,
        uint32_t sector_num) {
    status_t ret;
    uint32_t dest;
    uint32_t size;

    dest = sector_index << 12;
    size = sector_num << 12;
    ret = FLASH_DRV_EraseSector(&flashSSDConfig, dest, size);
    if (ret != STATUS_SUCCESS)
    {
        return ret;
    }

    /* Disable Callback */
    flashSSDConfig.CallBack = NULL_CALLBACK;

    /* Verify the erase operation at margin level value of 1, user read */
    ret = FLASH_DRV_VerifySection(&flashSSDConfig, dest, size / FTFx_DPHRASE_SIZE, 1u);
    if (ret != STATUS_SUCCESS)
    {
        return ret;
    }

    return STATUS_SUCCESS;
}

/*
 * \brief   向PFlash写入数据
 * \param   address             写入数据的地址,必须是8的倍数
 * \param   size                写入数据的字节数
 * \param   sourceBuffer[in]    要写入的数据
 * \param   failAddr[out]       返回写入数据失败的地址
 * \retval
 */
status_t flash_write_PFLASH(uint32_t    address,
                            uint32_t    size,
                            uint8_t    *p_sourceBuffer,
                            uint32_t   *p_failAddr)
{
    status_t ret;
    /* 向FLASH中写入数据 */
    ret = FLASH_DRV_Program(&flashSSDConfig, address, size, p_sourceBuffer);
    if (ret != STATUS_SUCCESS)
    {
        return ret;
    }

    /* 校验写入的数据 */
    ret = FLASH_DRV_ProgramCheck(&flashSSDConfig, address, size,
            p_sourceBuffer, p_failAddr, 1u);
    if (ret != STATUS_SUCCESS)
    {
        return ret;
    }
    return STATUS_SUCCESS;
}

/**
 * \brief   向EEPROM中写入数据
 *
 * \param   offset          写入数据地址与EEPROM首地址的偏移量
 * \param   sourceBuffer    要写入的数据
 * \param   len             写入数据的长度
 *
 * \retval  STATUS_SUCCESS  写入成功
 *          STATUS_ERROR    写入失败
 */
status_t flash_write_EEPROM(uint32_t    offset,
                            uint8_t    *sourceBuffer,
                            uint32_t    len)
{
    status_t ret;
    /* 向EEPROM中写入数据 */
     if (flashSSDConfig.EEESize != 0u)
     {
         uint32_t address;
         address = flashSSDConfig.EERAMBase + offset;
         ret = FLASH_DRV_EEEWrite(&flashSSDConfig, address, len, sourceBuffer);
         if (ret != STATUS_SUCCESS)
         {
             return ret;
         }

         /* 校验写入的数据 */
         if (*((uint32_t *)sourceBuffer) != *((uint32_t *)address))
         {
             return STATUS_ERROR;
         }
     }
     return STATUS_SUCCESS;
}

/**
 * \brief   FLASH操作成功中断服务函数
 */
void CCIF_Handler(void)
{
  /* 关闭flash写入完成中断 */
  FTFx_FCNFG &= (~FTFx_FCNFG_CCIE_MASK);

  return;
}

/**
 * \brief   写入flash前的回掉函数
 * \details 在向FALSH写入数据前,会先调用该函数
 */
START_FUNCTION_DEFINITION_RAMSECTION
void CCIF_Callback(void)
{
  /* 使能FLASH写入完成中断 */
  if ((FTFx_FCNFG & FTFx_FCNFG_CCIE_MASK) == 0u)
  {
      FTFx_FCNFG |= FTFx_FCNFG_CCIE_MASK;
  }
}
END_FUNCTION_DEFINITION_RAMSECTION
