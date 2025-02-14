#include <stdint.h>
#include <stdlib.h>
#include "tm4c1294ncpdt.h"

#define CLOCKS_POR_PERIODO 80000
#define CLOCKS_POR_PORCENTO_DE_PERIODO (CLOCKS_POR_PERIODO/100)
#define TENSAO_MEIO 2048  

// Funções do utils.s
void PLL_Init(void);
void SysTick_Init(void);
void SysTick_Wait1ms(uint32_t delay);
void SysTick_Wait1us(uint32_t delay);

void GPIO_Init(void);
void escrevePortE(uint32_t entrada);
void escrevePortF(uint32_t entrada);
void Timer_Init(void);
void Timer1A_Handler(void);

void UART_Init(void);
void limpaTelaUart(void);
void moveCursorInicioUart(void);
void verificaCaractereUart(void);
void escreveCaractereUart(uint32_t data);
void escrevePalavraUart(const char *palavra);
void escreveNumeroUart(uint32_t numero);

void ADC_Init(void);
void atualizaValorPotenciometro(void);

void estadoInicial(void);
void estadoAguardandoEscolhaControle(void);
void estadoAguardandoSentido(void);
void estadoVelocidadeTerminal(void);
void estadoVelocidadePotenciometro(void);

void verificaSentido(void);
void verificaControle(void);
void verificaVelocidadeTerminal(void);
void atualizaVelocidadePotenciometro(void);
void ativaMotor(void);
void desativaMotor(void);
void atualizaVelocidade(void);

typedef enum {
    ESTADO_INICIAL,
    AGUARDANDO_ESCOLHA_CONTROLE,
    AGUARDANDO_SENTIDO,
    VELOCIDADE_TERMINAL,
    VELOCIDADE_POTENCIOMETRO,
} Estados;

typedef enum {
    HORARIO, 
    ANTIHORARIO,
    AGUARDANDO_INDEFINIDO,
} Sentido;

typedef enum {
    TERMINAL, 
    POTENCIOMETRO,
    AGUARDANDO_CONTROLE,
} Controle;

typedef enum {
    SEMICICLO_POSITIVO = 1, 
    SEMICICLO_NEGATIVO = 0,
} EstadoPwm;

Estados estadoAtual = ESTADO_INICIAL;
Sentido sentidoMotor = AGUARDANDO_INDEFINIDO;
Controle tipoControle = AGUARDANDO_CONTROLE;
EstadoPwm estadoPwm = SEMICICLO_NEGATIVO;

uint16_t valorPotenciometro = 0; // Valor entre 0 e 4095
char caractereLidoUart = '\0';
uint32_t velocidadePercentual = 0; // Velocidade de 0 a 100%
uint32_t velocidadeAlvo = 0; // Velocidade alvo para aceleração/desaceleração

int main(void) {
    PLL_Init();
    SysTick_Init();
    GPIO_Init();
    UART_Init();
    ADC_Init();
    Timer_Init();
    
    while (1) {
        limpaTelaUart();
        moveCursorInicioUart();
        verificaCaractereUart();
        atualizaValorPotenciometro();
        atualizaVelocidade();
        switch(estadoAtual) {
            case ESTADO_INICIAL:
                estadoInicial();
								sentidoMotor = AGUARDANDO_INDEFINIDO;
								tipoControle = AGUARDANDO_CONTROLE;
								velocidadeAlvo = 0;
								velocidadePercentual = 0;
                break;
            case AGUARDANDO_ESCOLHA_CONTROLE:
                estadoAguardandoEscolhaControle();
                break;
            case AGUARDANDO_SENTIDO:
                estadoAguardandoSentido();
                break;
            case VELOCIDADE_TERMINAL:
                estadoVelocidadeTerminal();
                break;
            case VELOCIDADE_POTENCIOMETRO:
                estadoVelocidadePotenciometro();
                break;
            default:
                escrevePalavraUart("Estado invalido\n");
                estadoAtual = ESTADO_INICIAL;
                break;
        }
        SysTick_Wait1ms(100); 
    }
}

void estadoInicial(void) {
    escrevePalavraUart("Motor parado, pressione '*' para iniciar.\n");
    if(caractereLidoUart == '*') {
        estadoAtual = AGUARDANDO_ESCOLHA_CONTROLE;
    }
}

void estadoAguardandoEscolhaControle(void) {
    escrevePalavraUart("Controlar a velocidade pelo terminal (t) ou potenciometro (p)?\n");
    verificaControle();
    if(tipoControle == TERMINAL) {
        estadoAtual = AGUARDANDO_SENTIDO;
    } else if(tipoControle == POTENCIOMETRO) {
        estadoAtual = VELOCIDADE_POTENCIOMETRO;
        ativaMotor();
    }
}

void estadoAguardandoSentido(void) {
    escrevePalavraUart("horario (h) ou anti-horario (a)\n");
    verificaSentido();
    if(sentidoMotor != AGUARDANDO_INDEFINIDO) {
        estadoAtual = VELOCIDADE_TERMINAL;
        ativaMotor();
    }
}

void estadoVelocidadeTerminal(void) {
    verificaVelocidadeTerminal();
    if(caractereLidoUart == 's') {
        desativaMotor();
        estadoAtual = ESTADO_INICIAL;
        return;
    }
    if(sentidoMotor == HORARIO) {
        escrevePalavraUart("Sentido horario (h)\n");
    } else {
        escrevePalavraUart("Sentido anti-horario (a)\n");
    }
    escrevePalavraUart("Controle pelo terminal. Motor girando a ");
    escreveNumeroUart(velocidadePercentual);
    escrevePalavraUart("% da velocidade maxima.\n");
    escrevePalavraUart("Escolha a velocidade da rotacao do motor: \n");
    escrevePalavraUart("5 - 50% da velocidade\n");
    escrevePalavraUart("6 - 60% da velocidade\n");
    escrevePalavraUart("7 - 70% da velocidade\n");
    escrevePalavraUart("8 - 80% da velocidade\n");
    escrevePalavraUart("9 - 90% da velocidade\n");
    escrevePalavraUart("0 - 100% da velocidade\n");
}

void estadoVelocidadePotenciometro(void) {
    if(caractereLidoUart == 's') {
        desativaMotor();
        estadoAtual = ESTADO_INICIAL;
        return;
    }
    if(valorPotenciometro < TENSAO_MEIO) {
        sentidoMotor = HORARIO;
        velocidadeAlvo = 100 * (TENSAO_MEIO - valorPotenciometro) / TENSAO_MEIO;
    } else {
        sentidoMotor = ANTIHORARIO;
        velocidadeAlvo = 100 * (valorPotenciometro - TENSAO_MEIO) / TENSAO_MEIO;
    }
    escrevePalavraUart("Controle pelo potenciometro. Motor girando a ");
    escreveNumeroUart(velocidadePercentual);
    escrevePalavraUart("% da velocidade maxima.\n");
    if(sentidoMotor == HORARIO) {
        escrevePalavraUart("Sentido horario (h)\n");
    } else {
        escrevePalavraUart("Sentido anti-horario (a)\n");
    }
}

void verificaSentido(void) {
    if(caractereLidoUart == 'h') {
        sentidoMotor = HORARIO;
    } else if(caractereLidoUart == 'a') {
        sentidoMotor = ANTIHORARIO;
    }
}

void verificaControle(void) {
    if(caractereLidoUart == 't') {
        tipoControle = TERMINAL;
    } else if(caractereLidoUart == 'p') {
        tipoControle = POTENCIOMETRO;
    }
}

void verificaVelocidadeTerminal(void) {
    switch(caractereLidoUart) {
        case '5': velocidadeAlvo = 50; break;
        case '6': velocidadeAlvo = 60; break;
        case '7': velocidadeAlvo = 70; break;
        case '8': velocidadeAlvo = 80; break;
        case '9': velocidadeAlvo = 90; break;
        case '0': velocidadeAlvo = 100; break;
    }
}

void atualizaVelocidade(void) {
    if(velocidadePercentual < velocidadeAlvo) {
        velocidadePercentual++;
    } else if(velocidadePercentual > velocidadeAlvo) {
        velocidadePercentual--;
    }
}

void ativaMotor(void) {
    escrevePortF(0x04);
    estadoPwm = SEMICICLO_NEGATIVO;
    escrevePortE(0x00);
    TIMER1_TAILR_R = 100 * CLOCKS_POR_PORCENTO_DE_PERIODO;
    TIMER1_ICR_R |= 0x01;
    TIMER1_CTL_R |= 0x01;
}

void desativaMotor(void) {
    escrevePortF(0x00);
    estadoPwm = SEMICICLO_NEGATIVO;
    escrevePortE(0x00);
    TIMER1_CTL_R &= (~0x01u);
}

void UART_Init(void) {
    uint32_t UART0 = SYSCTL_RCGCUART_R0;
    SYSCTL_RCGCUART_R = UART0;
    while ((SYSCTL_RCGCUART_R & UART0) != UART0) {
    }
    
    UART0_CTL_R = 0;
    UART0_IBRD_R = 260;
    UART0_FBRD_R = 27;
    UART0_LCRH_R = 0x72;
    UART0_CC_R = 0;
    UART0_CTL_R = 0x301;
}

void verificaCaractereUart(void) {
    if ((UART0_FR_R & 0x10) == 0x10) {
        return;
    }
    
    caractereLidoUart = UART0_DR_R;
}

void escreveCaractereUart(uint32_t data) {
    while ((UART0_FR_R & 0x20) == 0x20) {};
    
    UART0_DR_R = data;
}

void limpaTelaUart(void) {
    escreveCaractereUart(0x1B);
    escreveCaractereUart('[');
    escreveCaractereUart('2');
    escreveCaractereUart('J');
}

void moveCursorInicioUart(void) {
    escreveCaractereUart(0x1B);
    escreveCaractereUart('[');
    escreveCaractereUart(';');
    escreveCaractereUart('H');
}

void escrevePalavraUart(const char *palavra) {
    while (*palavra != '\0') {
        escreveCaractereUart(*palavra);
        ++palavra;
    }
}

void escreveNumeroUart(uint32_t numero) {
    char digitos_invertidos[16] = {'0'};
    uint32_t numero_digitos = 0;
    
    while (numero > 0) {
        digitos_invertidos[numero_digitos] = '0' + numero % 10;
        numero /= 10;
        ++numero_digitos;
    }
    if (numero_digitos == 0) {
        numero_digitos = 1;
    }
    
    while (numero_digitos > 0) {
        --numero_digitos;
        escreveCaractereUart(digitos_invertidos[numero_digitos]);
    }
}

void ADC_Init(void) {
    uint32_t adc0 = SYSCTL_RCGCADC_R0;
    SYSCTL_RCGCADC_R = adc0;
    while ((SYSCTL_PRADC_R & adc0) != adc0) {
    }
    
    ADC0_PC_R = 0x07;
    ADC0_SSPRI_R = (0 << 12) | (1 << 8) | (2 << 4) | 3;
    ADC0_ACTSS_R = 0;
    ADC0_EMUX_R = 0;
    ADC0_SSMUX3_R = 9;
    ADC0_SSCTL3_R = 6;
    ADC0_ACTSS_R = 8;
}

void atualizaValorPotenciometro(void) {
    ADC0_PSSI_R = 8;

    if (ADC0_RIS_R != 8) {
        return;
    }
    
    uint32_t novoValor = ADC0_SSFIFO3_R;
    ADC0_ISC_R = 8;
    if ((novoValor - valorPotenciometro) < 20) {
        return;
    }
    valorPotenciometro = novoValor;
}

void Timer_Init(void) {
    SYSCTL_RCGCTIMER_R |= 0x02;
    while ((SYSCTL_PRTIMER_R & 0x02) == 0) {
    }

    TIMER1_CTL_R &= ~(0x01u);
    TIMER1_CFG_R &= ~(0x07u);
    TIMER1_TAMR_R = (TIMER1_TAMR_R & ~(0x03u)) | 0x02;
    TIMER1_TAILR_R = 100 * CLOCKS_POR_PORCENTO_DE_PERIODO;
    TIMER1_TAPR_R = 0;
    TIMER1_ICR_R |= 0x01;
    TIMER1_IMR_R |= 0x01;
    NVIC_PRI5_R = 4u << 13;
    NVIC_EN0_R = 1u << 21;
}

void Timer1A_Handler(void) {
    uint32_t tempo = 0;
    if (velocidadePercentual == 0) {
        estadoPwm = SEMICICLO_NEGATIVO;
        tempo = 100 * CLOCKS_POR_PORCENTO_DE_PERIODO;
    } else if (velocidadePercentual == 100) {
        estadoPwm = SEMICICLO_POSITIVO;
        tempo = 100 * CLOCKS_POR_PORCENTO_DE_PERIODO;
    } else {
        if (estadoPwm == SEMICICLO_NEGATIVO) {
            estadoPwm = SEMICICLO_POSITIVO;
            tempo = velocidadePercentual * CLOCKS_POR_PORCENTO_DE_PERIODO;
        } else {
            estadoPwm = SEMICICLO_NEGATIVO;
            tempo = (100 - velocidadePercentual) * CLOCKS_POR_PORCENTO_DE_PERIODO;
        }
    }

    if(sentidoMotor == HORARIO) {
        escrevePortE((unsigned) estadoPwm);
    } else {
        escrevePortE((unsigned) estadoPwm << 1);
    }

    TIMER1_TAILR_R = tempo;
    TIMER1_ICR_R |= 0x01;
}
