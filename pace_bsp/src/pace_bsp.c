//#include "pace_bsp.h"
#if 0
#include "jrd_lcd.h"
#include "backlight.h"
#include "key.h"
#include "bmt.h"
//#include "rtc.h"
#include "inv_mpu.h"
#include "sensors.h"
#include "hardware_version.h"
#include "bq27421.h"
#include "shex_handler.h"
#include "maincode_version_string.h"
#include "MT38M.h"
#include "jrd_vibrator.h"

/***  0??=    273.15K,  our data unit is 0.1k         temp =  data/10   -  273.15  *****/
#define BQ_POWEROFF_TEMP_H                        3332      //60??=     3331.5
#define BQ_POWEROFF_TEMP_L                         2531      //-20??
#define BQ_DISABLE_CHARGE_TEMP_H            3182      //45??
#define BQ_DISABLE_CHARGE_TEMP_L             2731      //0??
#define BQ_ENABLE_CHARGE_TEMP_42             3152      //42??
#define BQ_ENABLE_CHARGE_TEMP_3               2761      //3??

extern void MPU_INT_SET(FunctionalState state);
extern volatile LCD_ID lcd_id_config = LCD_ID_AUO;
/* Factory reset flag, for main factory menu. */
static char Temp_Charge_Control_flag = 0;

static uint32_t ZCV__value_Table[51] = {3400, 3488, 3560, 3607, 3615,           // each voltage data 2% , 0%  -- 98%
                                                   3623, 3627, 3640, 3653, 3664,
                                                   3674, 3680, 3686, 3693, 3700,
                                                   3706, 3715, 3717, 3726, 3733,
                                                   3740, 3749, 3755, 3765, 3772,
                                                   3783, 3788, 3800, 3817, 3830,
                                                   3846, 3860, 3874, 3886, 3914,
                                                   3924, 3950, 3964, 3985, 4005,
                                                   4030, 4047, 4070, 4091, 4106, 
                                                   4136, 4150, 4181, 4196, 4220, 4350};


/**
  * @brief  To get the type of boot up based on priority(From high to low).
  * @param  None
  * @retval the type of boot up.
  */

BootUpTypeEnum jrd_boot_up_type(void)
{
    static BootUpTypeEnum boot_type = BOOT_UP_MAX_MODE;

    if ( BOOT_UP_MAX_MODE == boot_type )
    {
        if (boot_key_state() == RESET) /* Key */
        {
            boot_type = BOOT_UP_BY_KEY;
        }
        else if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST) == SET) /* Software reset */
        {
            boot_type = BOOT_UP_BY_SW;
            __HAL_RCC_CLEAR_RESET_FLAGS();
        }
        else if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) == SET) /* Independent watchdog */
        {
            boot_type = BOOT_UP_BY_IWDG;
            __HAL_RCC_CLEAR_RESET_FLAGS();
        }
        else if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST) == SET) /* Window watchdog */
        {
            boot_type = BOOT_UP_BY_WWDG;
            __HAL_RCC_CLEAR_RESET_FLAGS();
        }
        else if (boot_charge_state() == RESET) /* Charging */
        {
            boot_type = BOOT_UP_BY_CHARGING;
        }
        else
        {
            boot_type = BOOT_UP_NULL;
        }
    }

    return boot_type;
}

static void Vibrator_work_at_first(void)
{
    //jrd_vibrator_start(50);
    vibrator_off();
    vibrator_on();
    jrd_ms_delay(250);
    vibrator_off();
}

static void Jrd_lowlevel_hold(void)
{ 
    uint32_t voltage;
        
    do{
        voltage = bq27421_Voltage();
    }while (voltage < 2850);
}
static void Jrd_Poweroff_Temp(BootUpTypeEnum boot_type, uint32_t Temperature)
{
    if((Temperature > BQ_POWEROFF_TEMP_H) || (Temperature < BQ_POWEROFF_TEMP_L))   // If  temp > 60??or   temp < -20??
    { 
        turn_off_one_second();
        jrd_ms_delay(30);  
        turn_off_one_second();
    }
}

static void Jrd_Charging_Control(uint32_t Temperature)
{
    if ((Temperature > BQ_DISABLE_CHARGE_TEMP_L) && (Temperature < BQ_DISABLE_CHARGE_TEMP_H))  // If   0??<  temp <    45??
    {
        if (Temp_Charge_Control_flag)
        {
             if ((Temperature > BQ_ENABLE_CHARGE_TEMP_3) && (Temperature < BQ_ENABLE_CHARGE_TEMP_42))  // If   3??<  temp <    42??
            {
                 Charger_IC_enable();        //enable  charge IC
                 Temp_Charge_Control_flag = 0;
            }
        }
    }
    else
    {    
        Charger_IC_disable();     //disable  charge IC
        Temp_Charge_Control_flag = 1;
    }
}

static void Jrd_Power_by_Key_Control(uint32_t Temperature)
{
    if(get_bmt_charger_status() == BMT_CHARGER_OUT){  //not in charging  
          if ((Temperature > BQ_DISABLE_CHARGE_TEMP_L) && (Temperature < BQ_DISABLE_CHARGE_TEMP_H)){  // If   0??<  temp <    45??
            if (Temp_Charge_Control_flag){
                if ((Temperature > BQ_ENABLE_CHARGE_TEMP_3) && (Temperature < BQ_ENABLE_CHARGE_TEMP_42)) {  // If   3??<  temp <    42??                             
                    Charger_IC_enable();        //enable  charge IC
                    Temp_Charge_Control_flag = 0;
                }
            }
         }
         else{     
             Temp_Charge_Control_flag = 1;
             Charger_IC_disable();     //disable  charge IC
         }
         if((bq27421_Voltage() < (uint32_t)TERMINAL_VOLTAGE) &&(bq27421_StateOfCharge() == 0)) 
          {
             /* If key down, it cannot be power off, poll until power key up. */
             while (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_15) == RESET) {
                  /* To avoid of key jitter. */
                 jrd_ms_delay(5);
             }
             printf("\r\n SHEXE_POWER_OFF_VOLTAGE_TOO_LOW, POWER OFF!");
             shex_power_off(SHEXE_POWER_OFF_VOLTAGE_TOO_LOW);
         }
     }
     else{                        //BMT_CHARGER_IN    
         if ((Temperature > BQ_DISABLE_CHARGE_TEMP_L) && (Temperature < BQ_DISABLE_CHARGE_TEMP_H)){// If   0??<  temp <    45??
              if (Temp_Charge_Control_flag){
                if ((Temperature > BQ_ENABLE_CHARGE_TEMP_3) && (Temperature < BQ_ENABLE_CHARGE_TEMP_42)) {  // If   3??<  temp <    42??                                  
                    Charger_IC_enable();        //enable  charge IC
                    Temp_Charge_Control_flag = 0;
                }
            }
         }
         else{
             Charger_IC_disable();
             Temp_Charge_Control_flag = 1;
         }
     }
}

extern bmt_struct bmt_data;
extern uint8_t lcd_sleep_status;
uint8_t fuel_gauge_check_and_update(bmt_struct *bmt_init_data)
{
    bmt_struct bmt_status;
    uint32_t voltage = 0;
    uint8_t num = 0, disp_soc = 0, soc_level = 100, soc_compare = 0;
    
        bmt_status.volt = bq27421_Voltage();
        bmt_status.disp_level = (uint8_t)bq27421_StateOfCharge();
        bmt_status.status = get_bmt_charger_status();
        bmt_status.plg_status = get_bmt_plug_status();

        voltage = bmt_status.volt;
        disp_soc = bmt_status.disp_level;

        if ((voltage > 4800) || (voltage < 1000) ||(disp_soc > 100))
        {
            return 0;
        }
     //   printf("\n\r get_bmt_status_begain - > voltage = %dmV      soc = %d", voltage, disp_soc);
        if (bmt_status.plg_status)
        {
            voltage = voltage -170 + (uint8_t)(disp_soc * 0.3 );//when charging , compare with 50mA Load ,  soc = 0% the voltage rise 170mv    soc = 98% the voltage rise 120mv
        }
        if(lcd_sleep_status ==1)
        {
            voltage = voltage - 20;
        }
        for (num = 0; num <51; num++)
        {
            if (voltage < ZCV__value_Table[num])
            {
                soc_level = num * 2;
                printf("the ZCV_Table soc = %d", soc_level);
                break;
            }
        }
        if (soc_level > disp_soc){
            soc_compare = soc_level -disp_soc;
        }
        else{
            soc_compare = disp_soc -soc_level;
        }
        if (soc_compare < 15)
        {
       //     printf("\n\r ************************fuel_gauge_check_and_update   successe!*************************");
            bmt_init_data->volt=bmt_status.volt;
            bmt_init_data->disp_level=bmt_status.disp_level;
            bmt_init_data->status=bmt_status.status;
            bmt_init_data->plg_status=bmt_status.plg_status;
            return 1;
        }
        else{
            return 0;
      //      printf("\n\r ************************fuel_gauge_check_and_update   faile!*************************");
        }                        
}

void fuel_gauge_update_first(void)
{
    uint8_t update_num;
    bmt_struct bmt_status;

    for (update_num = 0; update_num < 5; update_num++){
        if (fuel_gauge_check_and_update(&bmt_status))
        {
            bmt_data = bmt_status;
        //    printf("\n\r bmt_data.voltage = %d   soc = %d", bmt_data.volt, bmt_data.disp_level);
            break;
        }
    }
}
void Jrd_lowlevel_Vol_Temp_Control( BootUpTypeEnum boot_type)  
{
    uint32_t Temperature;
    
    Temperature = bq27421_Temperature();
    
    if ((bq27421_isOk() == MMI_TRUE) &&(Temperature > 2000))
    {
        Jrd_Poweroff_Temp(boot_type, Temperature);                      // If  temp > 60??or   temp < -20??
        
        if (BOOT_UP_BY_CHARGING == boot_type)                             //power on by charging
        {  
            Jrd_Charging_Control(Temperature);
        }
        else                                                                                                     //power on not by charging
        {  
            Jrd_Power_by_Key_Control(Temperature);
        }
    }
}
void jrd_lowlevel_init( BootUpTypeEnum boot_type )
{
    uint8_t config_id[4]={0};
    
  #ifdef MT38M_USE_MUX	
    MT38M_MuxInit();
  #endif
  
    BSP_LCD_GPIO_28V(1);
    //+linx 20161229 add watch dog
    extern BootUpTypeEnum boot_up_mode;
    if(BOOT_UP_BY_IWDG == boot_up_mode){
        printf("IWDG reset skip Vibrator_work_at_first()\r\n");
    }else{
    //-linx
        Vibrator_work_at_first();
    }
    sensor_I2C_handle_init();
    bq27421_init();
    Jrd_lowlevel_hold();
    fuel_gauge_update_first();
    Jrd_lowlevel_Vol_Temp_Control(boot_type);

    /* If not charger and is mini bootup. Must after Nor_Flash_Init(). */
    if ((custpack_get_mini_bootup_mode() == 1) && (get_bmt_charger_status() == BMT_CHARGER_OUT)) {
        /* After reset, need 1-5 seconds to read the correct SOC of fuel guage. */
        fuel_guage_reset();
    }

    if(TP_Read_Config_ID(config_id))
    {
    //log_init is runned after this init fun,but uart init has been runned   //modify by Jun
    //JRD_drv_printf("TP Config_ID =0x%x%x%x%x\n",config_id[0], config_id[1], config_id[2], config_id[3]);
     printf("TP Config_ID =0x%x%x%x%x\n",config_id[0], config_id[1], config_id[2], config_id[3]);
      if(config_id[1] == 0x30)
      {
         lcd_id_config = LCD_ID_EDO;
      }
      else if(config_id[1] == 0x31)
      {
         lcd_id_config = LCD_ID_AUO;
      }
      else
      {
         lcd_id_config = LCD_ID_NONE;
      }
    //log_init is runned after this init fun,but uart init has been runned   //modify by Jun
    //JRD_drv_printf("set lcd_id_config =%d\n",lcd_id_config);  
     printf("set lcd_id_config =%d\n",lcd_id_config);  
    }
    else
    {
         lcd_id_config = LCD_ID_NONE;
         printf("!!!!!!Read TP I2C Config error Read TP ID set =%d\n",lcd_id_config);
     }

   if((lcd_id_config == LCD_ID_AUO)||(lcd_id_config == LCD_ID_EDO)) 
   {
	   LCD_Init();

   }
}
void BSP_LCD_GPIO_28V(uint8_t value)
{
  GPIO_InitTypeDef  gpio_init_structure;
  __HAL_RCC_GPIOH_CLK_ENABLE();

  /* Configure the GPIO on PH3 for 2.8V enable*/
  gpio_init_structure.Pin   = GPIO_PIN_3;
  gpio_init_structure.Mode  = GPIO_MODE_OUTPUT_PP;
  gpio_init_structure.Pull  = GPIO_PULLUP;
  gpio_init_structure.Speed = GPIO_SPEED_HIGH;
  HAL_GPIO_Init(GPIOH, &gpio_init_structure);

  if(value)
      HAL_GPIO_WritePin(GPIOH, GPIO_PIN_3, GPIO_PIN_SET);  //2.8V _Enable
  else
      HAL_GPIO_WritePin(GPIOH, GPIO_PIN_3, GPIO_PIN_RESET);  //2.8V _Disable
}
/*
 * Function:     Read_TP_ID_GPIO
 * Arguments:    NULL
 * Return Value: GPIO_PinState:
 *               GPIO_PIN_SET or GPIO_PIN_RESET         
 * Description:   Read TP ID GPIO Status
 */
GPIO_PinState Read_TP_ID_GPIO(void)
{
  GPIO_InitTypeDef  gpio_init_structure;
  __HAL_RCC_GPIOC_CLK_ENABLE();
  gpio_init_structure.Pin   = GPIO_PIN_0;
  gpio_init_structure.Mode  = GPIO_MODE_INPUT;
  gpio_init_structure.Pull  = GPIO_PULLUP;
  gpio_init_structure.Speed = GPIO_SPEED_HIGH;
  HAL_GPIO_Init(GPIOC, &gpio_init_structure);
  return HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_0);
}
 #endif
