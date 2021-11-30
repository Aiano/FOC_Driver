/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "pwm.h"
#include "FOC.h"
#include "encoder.h"
#include "pid.h"
#include "uart_parser.h"
#include "vl53l0x.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"


/* USER CODE BEGIN PV */
FOCDriverType driver1;
FOCDriverType driver2;

FOCEncoderType encoder1;
FOCEncoderType encoder2;

pid_parameters pid1;
pid_parameters pid2;

float given_basic_voltage = 3;
float given_voltage = 0;
// initial angles
const float init_angle1 = 4.2;
const float init_angle2 = 3.2;

// given angles from uart
// range from -0.5 to 0.5 ( left to right )
float given_angle1 = 0;
// range from -1.0 to 1.0 ( down to up )
float given_angle2 = 0;

// foc input angles
float now_angle1 = 0;
float now_angle2 = 0;

// read angles from encoder
float real_angle1 = 0;
float real_angle2 = 0;

// data of vl53l0x
struct VL53L0xData data;
uint16_t dist;
char dist_string[20];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
void InitFOCDriver();

void InitFOCEncoder();

void InitPID();

void change_angle_A(char *);

void change_angle_B(char *);

void set_laser_status(char *);

void get_distance(char *);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_TIM1_Init();
    MX_TIM2_Init();
    MX_ADC1_Init();
    MX_ADC2_Init();
    MX_I2C1_Init();
    MX_TIM3_Init();
    MX_USART2_UART_Init();
    /* USER CODE BEGIN 2 */

    // init FOC driver
    InitFOCDriver();

    InitFOCEncoder();
    InitPID();

    // init uart parser
    uart_parser_init(&huart2);
    // subscribe command
    uart_parser_add_command('A', change_angle_A, "change angle A");
    uart_parser_add_command('B', change_angle_B, "change angle B");
    uart_parser_add_command('L', set_laser_status, "toggle laser");
    uart_parser_add_command('D', get_distance, "get distance");

    // init vl53l0x
    VL53L0x_init(&hi2c1);
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1) {
        real_angle1 = get_encoder_angle(&encoder1);
        real_angle2 = get_encoder_angle(&encoder2);

        float u1 = pid_u(given_angle1 + init_angle1, real_angle1, pid1);
        float u2 = pid_u(given_angle2 + init_angle2, real_angle2, pid2);

        now_angle1 += u1;
        now_angle2 += u2;

        FOC_Open_Loop_Angle_Control(&driver1, now_angle1, 5);
        FOC_Open_Loop_Angle_Control(&driver2, now_angle2, 5);

        HAL_Delay(5);

        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        Error_Handler();
    }
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

void InitFOCDriver() {
    driver1.voltage_power_supply = 12;
    driver1.phaseATIMHandle = &htim1;
    driver1.phaseBTIMHandle = &htim1;
    driver1.phaseCTIMHandle = &htim1;
    driver1.phaseAChannel = TIM_CHANNEL_1;
    driver1.phaseBChannel = TIM_CHANNEL_2;
    driver1.phaseCChannel = TIM_CHANNEL_3;
    driver1.modulationType = SinePWM;
    driver1.pwm_tim_counter_period = 200;
    driver1.polar_pair_number = 7;
    FOC_Driver_Init(&driver1);
    HAL_GPIO_WritePin(EN1_GPIO_Port, EN1_Pin, GPIO_PIN_SET);

    driver2.voltage_power_supply = 12;
    driver2.phaseATIMHandle = &htim3;
    driver2.phaseBTIMHandle = &htim3;
    driver2.phaseCTIMHandle = &htim3;
    driver2.phaseAChannel = TIM_CHANNEL_1;
    driver2.phaseBChannel = TIM_CHANNEL_2;
    driver2.phaseCChannel = TIM_CHANNEL_3;
    driver2.modulationType = SinePWM;
    driver2.pwm_tim_counter_period = 200;
    driver2.polar_pair_number = 7;
    FOC_Driver_Init(&driver2);
    HAL_GPIO_WritePin(EN2_GPIO_Port, EN2_Pin, GPIO_PIN_SET);
}

void InitFOCEncoder() {
    encoder1.encoder_bit_number = 12;
    encoder1.encoderADCHandle = &hadc1;
    encoder1.offset_angle = 0;

    encoder2.encoder_bit_number = 12;
    encoder2.encoderADCHandle = &hadc2;
    encoder2.offset_angle = 0;
}

void InitPID() {
    pid1.Kp = 0.03;
    pid1.Ki = 0;
    pid1.Kd = 0;
    pid1.use_p = true;
    pid1.use_i = false;
    pid1.use_d = false;

    pid2.Kp = 0.03;
    pid2.Ki = 0;
    pid2.Kd = 0;
    pid2.use_p = true;
    pid2.use_i = false;
    pid2.use_d = false;
}

void change_angle_A(char *cmd) {
    uart_parser_scalar(&given_angle1, cmd);
}

void change_angle_B(char *cmd) {
    uart_parser_scalar(&given_angle2, cmd);
}

void set_laser_status(char *cmd) {
    if (cmd[0] == '\n') {
        HAL_GPIO_TogglePin(Laser_Triode_GPIO_Port, Laser_Triode_Pin);
    } else if (cmd[0] == 'O') {
        HAL_GPIO_WritePin(Laser_Triode_GPIO_Port, Laser_Triode_Pin, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(Laser_Triode_GPIO_Port, Laser_Triode_Pin, GPIO_PIN_RESET);
    }
}

void get_distance(char *) {
    VL53L0x_StartConversion(&hi2c1);
    VL53L0x_ReadDistance(&hi2c1, &data);
    dist = data.dist;
    uart_parser_output_number("distance", dist, &huart2);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

#pragma clang diagnostic pop