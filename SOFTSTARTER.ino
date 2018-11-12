#include "TimerOne.h"
#include <RotaryEncoder.h>
#include <LiquidCrystal.h> //Inclui a biblioteca do LCD   

LiquidCrystal lcd(4, 5, 9, 10, 11, 12);       
 
RotaryEncoder encoder(A3, A4);             //Pinos de ligacao do encoder ]


#define Main       0   
#define SetSubida  1 
#define SetDescida 2         
#define Set        3           
#define Subindo    4      
#define Descendo   5       
#define Merda      6 
#define SetModo    7       
#define POT        8         
 
      
#define PinDetector                    3   //  Pino do detector de Zero   
#define PinPot                         A7   //  Pino do detector de Zero  
#define PinPulsoTiristor               2   //  Pino do pulso no tiristor    
#define SobreCorrente600mA             8   //  Pino do sinal de sobrecorrente de 600mA         
#define SobreCorrente800mA             7   //  Pino do sinal de sobrecorrente de 800mA     
#define LedPin                         13  //  pra ver o toggle do Timer       
#define BotEncoder                     6  //  Pino do botão do Encoder         
#define ANALOGSobreCorrente600mA       A1  //  Pino analogico do sinal de sobrecorrente de 600mA        
#define ANALOGSobreCorrente800mA       A2  //  Pino analogico do sinal de sobrecorrente de 800mA   
#define PinoReleByPass                 A5  //  Pino analogico sendo usado como digital para controlar o relé de Bypass  

#define liga600ma                      digitalWrite(SobreCorrente600mA, HIGH);
#define liga800ma                      digitalWrite(SobreCorrente800mA, HIGH);
#define desliga600ma                   digitalWrite(SobreCorrente600mA, LOW);
#define desliga800ma                   digitalWrite(SobreCorrente800mA, LOW); 

#define TogglePinTeste                 PinTesteState = !PinTesteState;   digitalWrite(detectorDeZeroInvertido, ledState);  //  Função pra dar um Toggle no pino de teste 
#define ToggleLed                      ledState = !ledState;   digitalWrite(LedPin, ledState);                             //  Função pra dar um Toggle no LED 
#define LigaTiristor                   digitalWrite(PinPulsoTiristor, HIGH);                                               //  liga o pino do tiristor
#define DesligaTiristor                digitalWrite(PinPulsoTiristor, LOW);                                                //  Desliga o pino do tiristor   
#define LigaReleByPass                 analogWrite(PinoReleByPass, 1000); 
#define DesligaReleByPass              analogWrite(PinoReleByPass, 0);

#define MedicaoTempo1                  time1 = micros()/1000;  break; // faz medição do tempo no primeiro pulso recebido no detector de zero
#define MedicaoTempo2                  time2 = micros()/1000;  break; // faz medição do tempo no segundo pulso recebido no detector de zero  

#define PulsoTiristorConfig            0     
#define LarguraPulsoTiristorConfig     1  
#define TempoDeLigacaoConfig           2   
#define TempoDeDesligamentoConfig      3       
       
unsigned long 
time3,                //  Var para armazenar o tempo em milisegundos    
time4,                //  Var para armazenar o tempo em milisegundos
contador =0,           
maxValorAlcancado,    // maximo valor alcançado pela variavel contadora 
LarguraDePulsoValor = 20; // LARGURA DO PULSO EM GRAUS, DE 0 a 180    

double periodo = 0,time1,time2;  
 

       
unsigned int 
TempoPulsoTiristor = 0,   // Em microsegundos       
n=0,  
LarguraDePulso = 0,
valor = 0,        
newPosGlobal = 0,
tela = Main;     


unsigned long int  contDesbuga = 0;

bool flag600mA = 0; 
bool iniciaDown = 0; 
bool fezMedicaoFreq = 0; 
bool fezMedicaoFreq2 = 0;

 
unsigned long int 
medicaoFreq = 0,
medicaoFreq2 = 0,    
contadorTempoAtraso = 0, 
contadorTempo =0,  
contadorTempo2 =0,
contadorTempo3 =0,
contadorTempoDesliga =0,  
cont = 0,     
TempoDivisao = 0,
TempoDivisaoDesliga =0,
TempoPraLigar =10,
TempoPraLigarAntes =10, 
TempoPraDesligar =10,
TempoPraDesligarAntes = 10, 
TempoNewPos = 5;         

bool potAtiva = 0; 
     
char comunic;   

int divisaoAngulo = 181;  
 
bool ZeroDetectado = 0, // Flag do detector de zero  
Start = 0,               // Flag para iniciar o programa    
VerificaPeriodo = 0,    // Flag pra verificar o periodo  
VerificadoPeriodo = 0,  // Flag pra ver se o periodo já foi veficado   
ledState = 0,           // Flag pra ver o estado do LED    
PinTesteState = 0,      // Flag pra ver o estado do LED    
enablePulso = 1,         // Flag pra permitir ou não que seja dado o pulso no tiristor  
FuncionandoNominal = 0,
diminui = 0,
desliga = 0,
BotPressionado = 0,
AtualizaTela = 0,
vai=0,
atualizaTela=0,
deuMerda=0,
flagbot = 0,
Sobre600mA = 0,
foiMain = 0;         

static int pos = 0;
int newPos; 
int contVaiMain = 0;
int AnguloDeDisparo = 180;      // Var do angulo de disparo no Tiristor , entre 0 e 180
int contAtualiza = 0; 
  

byte SetaUp[8] = {
  0b00100, 
  0b01110,
  0b11111, 
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b00100
};      

#define PPM 0
#define PWM 1 

int MODO = PPM; 
 

 
void DetectorDeZero(){      // INTERRUPÇÂO PARA O PINO DO DETECTOR DE ZERO     
 
  verificaPinos(); 
  if(n<10){switch(n++){  
    case 2: MedicaoTempo1 
    case 3: MedicaoTempo2
  }}     

  if(fezMedicaoFreq&&(!fezMedicaoFreq)){
    fezMedicaoFreq2 = 1;
    medicaoFreq2 = millis();        
  }    

  if(!fezMedicaoFreq&&(!fezMedicaoFreq)){
    fezMedicaoFreq = 1;
    medicaoFreq = millis();  
  }  
  
  periodo = ( time2 - time1)/1000; // faz diferença dos dois tempos pra saber quanto tempo se passou   
  time3=0;                 // zera a variavel contadora do timer          
  DesligaTiristor          // desliga o pino do tiristor       
  enablePulso = 1;      // habilita para que seja dado o pulso a qualquer momento      
   
  if(FuncionandoNominal){ LigaReleByPass  }// Se chegar no valor nominal aciona o relé pra botar o motor em paralelo com a rede AC
  else                  {DesligaReleByPass }  
}    
 
 
 
void CallBackTimerOne(){   // INTERRUPÇÂO DO TIMER DE 2 KHZ   

 
  contAtualiza++;    
  if(contAtualiza>2000){
    atualizaTela = 0; 
    contAtualiza = 0;  
  }     
   
  verificaPinos();   
  if(((Start)||(diminui))&&(vai)&&(AnguloDeDisparo<155)){     
              
    if( (++time3) >= ( TempoPulsoTiristor ) && enablePulso && (AnguloDeDisparo<180) )    {  LigaTiristor    enablePulso = 0; } // liga o Gate do tiristor  
    if(AnguloDeDisparo<=0)    {  LigaTiristor    enablePulso = 0; } // liga o Gate do tiristor   
    if(MODO == PPM) {    if( (++time3) >= TempoPulsoTiristor + LarguraDePulso){  DesligaTiristor enablePulso = 0; } }                                // Desliga o Gate do tiristor  
    if(time3>maxValorAlcancado) maxValorAlcancado = time3;                                                                     // Atualiza sempre o maior valor alcançado pela variavel contadora    
  
  }   
  else {        
    DesligaTiristor 
  }

  if(FuncionandoNominal && (tela != Main) && (!foiMain)){
    if(contVaiMain++ > 6000){
      tela = Main;
      contVaiMain = 0; 
      lcd.clear();
      foiMain = 1;
    }       
  } 
 
 if(deuMerda){
    liga800ma 
    if(contVaiMain++ > 6000){
      tela = Main;
      contVaiMain = 0; 
      lcd.clear();
      deuMerda=0; 
      lcd.clear(); 
      desliga800ma
      desliga600ma 
    }       
  }
   
   if((Start) ){    
    vai = 1;   
    if(Sobre600mA&&!flag600mA) { liga600ma  flag600mA = 1; contadorTempo = 0; divisaoAngulo = abs(AnguloDeDisparo - 181); if(TempoPraLigar*3<50) { TempoPraLigar = TempoPraLigar*3; } else{ TempoPraLigar = 50; } } 
    
    if( (contadorTempo++ <= TempoDivisao ) &&!diminui)   {         // Contagem do tempo total de ligamento do motor
      if(contadorTempo2++ >= TempoDivisao/divisaoAngulo ) {                  // divide a Contagem do tempo total de ligamento do motor em 180 pra dar 180 decrementos no angulo de disparo          
         contadorTempo2=0; AnguloDeDisparo--;                      // decrementos no angulo de disparo           
         configura(PulsoTiristorConfig,AnguloDeDisparo);           // Faz nova configuração dos valores    
         configura(LarguraPulsoTiristorConfig,LarguraDePulsoValor);// Faz nova configuração dos valores   
         configura(TempoDeLigacaoConfig,TempoPraLigar);                 // Faz nova configuração dos valores  
         configura(TempoDeDesligamentoConfig,TempoPraDesligar);            // Faz nova configuração dos valores 
         atualizaTela=0;
        }      
      }         
      if((AnguloDeDisparo<=3) ) {           // Se passar da contagem de desligamento 
      Start = 0;  
      vai = 0;   
      contadorTempo =0; 
      FuncionandoNominal = 1;                                     // diz que está no valor nominal de corrente 
      atualizaTela=0;  
      lcd.clear();
      if(flag600mA) TempoPraLigar = TempoPraLigarAntes;    
      flag600mA = 0;
      Sobre600mA =0;    
      desliga800ma  
      desliga600ma
      contDesbuga = 0; 
      foiMain = 0;
      divisaoAngulo = 181; 
      contadorTempo2 = contadorTempo = contadorTempo3 = contadorTempoDesliga = 0;
      AnguloDeDisparo = 0;
    }    
  }    
  else if (diminui){
    if(Sobre600mA&&!flag600mA) { liga600ma flag600mA = 1; divisaoAngulo = (180 - AnguloDeDisparo ); if(TempoPraDesligar*3<50) TempoPraDesligar = TempoPraDesligar*3; else TempoPraDesligar = 50; } 
    if( (contadorTempoDesliga++ <= TempoDivisaoDesliga ) && diminui )   {
          if(contadorTempo3++ >= TempoDivisaoDesliga/181 ) {
             contadorTempo3=0; AnguloDeDisparo++;
             configura(PulsoTiristorConfig,AnguloDeDisparo);           // Faz nova configuração dos valores   
             configura(LarguraPulsoTiristorConfig,LarguraDePulsoValor);// Faz nova configuração dos valores  
             configura(TempoDeLigacaoConfig,TempoPraLigar);            // Faz nova configuração dos valores 
             configura(TempoDeDesligamentoConfig,TempoPraDesligar);            // Faz nova configuração dos valores  
            }
          } 
    if((AnguloDeDisparo>=177) ) { 
           contadorTempoDesliga = 0;
           Start = 0;
           contadorTempo3 =0;           
           diminui = 0;
           vai =0;  
           AnguloDeDisparo = 180;
           FuncionandoNominal=0;   
           tela = Main;  
           atualizaTela=0;       
           lcd.clear(); 
           if(flag600mA) TempoPraDesligar = TempoPraDesligarAntes;    
           flag600mA = 0;
           Sobre600mA =0; 
           desliga800ma 
           desliga600ma 
           foiMain = 0; 
           contadorTempo2 = contadorTempo = contadorTempo3 = contadorTempoDesliga = 0;
    }
  }   

    
  
  if ( (digitalRead(BotEncoder)==0)&&(!Start)&&(!diminui) ) {    // Se o botão do encoder foi pressionado, só pra testes por enquanto   
   atualizaTela=0;   
   switch(tela){  
    case Main:   
      if(newPosGlobal==0 && (!FuncionandoNominal)) { contadorTempo2 = contadorTempo = contadorTempo3 = contadorTempoDesliga = 0; AnguloDeDisparo = 179; tela=Subindo; Start=1; }  
      if(newPosGlobal==1) tela = Set;  
      if(newPosGlobal==2 && (FuncionandoNominal || Start)) { iniciaDown = 1; }
     
      break;  
    case Set:
      if(newPosGlobal==0) tela = Main;
      if(newPosGlobal==1) { tela = SetSubida;  encoder.setPosition (5); newPosGlobal = pos = newPos = 5; } 
      if(newPosGlobal==2) { tela = SetDescida; encoder.setPosition (5); newPosGlobal = pos = newPos = 5; }  
      if(newPosGlobal==3) { tela = SetModo;    encoder.setPosition (0); newPosGlobal = pos = newPos = 0; }    
      break;  
    case SetModo:
      tela = Set; 
      if(newPosGlobal == 0){  MODO = PPM; } 
      if(newPosGlobal == 1){  MODO = PWM; }       
      break; 
    case SetSubida:      
      tela = Main;
      TempoPraLigarAntes = TempoPraLigar = newPos;   
      encoder.setPosition (0);  newPosGlobal = pos = newPos = 0;
      break; 
    case SetDescida:  
      tela = Main;
      TempoPraDesligarAntes = TempoPraDesligar = newPos; 
      encoder.setPosition (0);  newPosGlobal = pos = newPos = 0;  
      break; 
   } 
    while ( (digitalRead(BotEncoder) == 0) && !Start)  {
         delay(10);  
    }                         
  }   
    
  encoder.tick();   
  newPos = encoder.getPosition();   
  if (pos != newPos) {
     atualizaTela=0;                     
     newPosGlobal = pos = newPos;        
     ToggleLed                
     Serial.print( time1);    Serial.print( '-'); 
     Serial.print( time2);         
     switch(tela){  
        case Main:       if(newPos > 2) { setaTudo(0); } if(newPos < 0){ setaTudo(2);  } break;  
        case Set:        if(newPos > 3) { setaTudo(0); } if(newPos < 0){ setaTudo(3);  } break; 
        case SetSubida:  if(newPos > 50){ setaTudo(5); } if(newPos < 5){ setaTudo(50); } break; 
        case SetDescida: if(newPos > 50){ setaTudo(5); } if(newPos < 5){ setaTudo(50); } break; 
        case SetModo:    if(newPos > 1){  setaTudo(0); } if(newPos < 0){ setaTudo(1);  } break;       
     }
  }  
}    
 
void setaTudo(int x){
   encoder.setPosition (x); newPosGlobal = pos = newPos = x; 
}
    
    
void setup() {  
  Serial.begin(38400);                                                              // configura a comunicação serial com 38400 bps  pra usar com BLUETOOTH 
  pinMode(PinDetector,INPUT_PULLUP);                                                // configura pino como entrada pull up
  pinMode(PinPulsoTiristor,OUTPUT);                                                 // configura pino como saida 
  pinMode(LedPin,OUTPUT);                                                           // configura pino como saida     
  pinMode(SobreCorrente600mA,INPUT);                                                // configura pino como entrada  pull up
  pinMode(SobreCorrente800mA,INPUT);                                                // configura pino como entrada  pull up  
  Timer1.initialize(500);                                                           // Inicializa o Timer1 e configura para um período de 100 microsegundos ou 10Khz 
  Timer1.attachInterrupt(CallBackTimerOne);                                         // Configura a função callback() como a função para ser chamada a cada interrupção do Timer1
  attachInterrupt(digitalPinToInterrupt(PinDetector), DetectorDeZero, FALLING);     // aciona interrupção para o pino  PinDetector pra ir quando o sinal cair (borda de descida) 
  lcd.begin(16, 2);                                                                 //Define o número de colunas e linhas do LCD        
  pinMode(BotEncoder, INPUT);                                                       // configura pino como entrada  
  lcd.createChar(1, SetaUp);
}  
  
  
void verificaPinos(){      // FAZ A LEITURA DOS PINOS JÁ QUE ELES NÃO POSSUEM INTERRUPÇÂO :( 
  if(analogRead(ANALOGSobreCorrente600mA) < 100 ) CallbackSobreCorrente600mA();  
  if(analogRead(ANALOGSobreCorrente800mA) < 400 ) CallbackSobreCorrente800mA();                        
}    
      

void CallbackSobreCorrente600mA(){ // Quando receber o sinal de que atingiu sobre corrente de 600mA entra nessa função 
      if (!flag600mA&&(Start||diminui)&&(AnguloDeDisparo<90)&&(AnguloDeDisparo>20)) Sobre600mA =1;         
} 
   
void CallbackSobreCorrente800mA(){ // Quando receber o sinal de que atingiu sobre corrente de 800mA entra nessa função 
   Start = 0;
   foiMain = 0; 
   diminui = 0; 
   vai =0; 
   AnguloDeDisparo = 180;
   FuncionandoNominal=0;  
   tela = Merda;     
   deuMerda = 1; 
   contVaiMain =0;
   lcd.clear();
}  
  
void configura(int configuracao, int x){ 
  switch(configuracao){ 
    case PulsoTiristorConfig:          TempoPulsoTiristor =  map(x, 0, 181, 0, maxValorAlcancado);      break;   // faz uma regra de 3 pra transformar o angulo dado em tempo da variavel contadora do timer       
    case LarguraPulsoTiristorConfig:   LarguraDePulso =  map(x, 0, 181, 0, maxValorAlcancado);          break;  // faz uma regra de 3 pra transformar o angulo dado em tempo da variavel contadora do timer     break;
    case TempoDeLigacaoConfig:         if(!flag600mA){if(x>=5&&x <=50) TempoDivisao = ( x/0.0005);} else { if(x>=5&&x <=50) TempoDivisao = ( x/0.0004);}                    break;  // faz uma regra de 3 pra transformar o angulo dado em tempo da variavel contadora do timer  break; 
    case TempoDeDesligamentoConfig:    if(x>=5&&x <=50) TempoDivisaoDesliga = ( x/0.0005);              break;  // faz uma regra de 3 pra transformar o angulo dado em tempo da variavel contadora do timer break;
  }
} 
 

void printSetatUp(int y){
  for(int x=0;x< y;x++) 
      lcd.write(1); 
}   
 
void switchBaixo10(int x){   
  if(x<10){      
      if(x==5) lcd.print('5');
      if(x==6) lcd.print('6');
      if(x==7) lcd.print('7');
      if(x==8) lcd.print('8');
      if(x==9) lcd.print('9');
      if(x<0)  lcd.print('0'); 
  } 
  else{
      lcd.print(x);         //Envia o texto entre aspas para o LCD  
  }
} 
     
  
                 
void loop() {          
  
 if(!atualizaTela) {  lcd.begin(16, 2);  lcd.clear(); atualizaTela=1; }    

 switch(tela){ 
  case Main:   
     lcd.setCursor(0, 0);        //Posiciona o cursor na coluna 3, linha 0;  
     lcd.print("START");         //Envia o texto entre aspas para o LCD    
     lcd.setCursor(7, 0);        //Posiciona o cursor na coluna 7, linha 0;  
     lcd.print("SET"); //Envia o texto entre aspas para o LCD  
     lcd.setCursor(11, 0);        //Posiciona o cursor na coluna 7, linha 0;  
     lcd.print("STOP"); //Envia o texto entre aspas para o LCD        
     if( newPosGlobal == 0 ) {  lcd.setCursor(0, 1); printSetatUp(5);  } 
     if( newPosGlobal == 1 ) {  lcd.setCursor(7, 1); printSetatUp(3);  }
     if( newPosGlobal == 2 ) {  lcd.setCursor(11,1); printSetatUp(4);  }
   
   
      
   break; 

   case Set:
     lcd.setCursor(0, 0);         //Posiciona o cursor na coluna 3, linha 0; 
     lcd.print("BACK");         //Envia o texto entre aspas para o LCD
     lcd.setCursor(5, 0);         //Posiciona o cursor na coluna 3, linha 0; 
     lcd.print("UP");         //Envia o texto entre aspas para o LCD
     lcd.setCursor(8, 0);         //Posiciona o cursor na coluna 3, linha 0; 
     lcd.print("DOWN");         //Envia o texto entre aspas para o LCD        
     lcd.setCursor(13, 0);         //Posiciona o cursor na coluna 3, linha 0; 
     lcd.print("MOD");         //Envia o texto entre aspas para o LCD        
     if( newPosGlobal == 0 ) {  lcd.setCursor(0, 1);  printSetatUp(4); }
     if( newPosGlobal == 1 ) {  lcd.setCursor(5, 1);  printSetatUp(2); }  
     if( newPosGlobal == 2 ) {  lcd.setCursor(8, 1);  printSetatUp(4); } 
     if( newPosGlobal == 3 ) {  lcd.setCursor(13, 1); printSetatUp(3); }     
     break;

     case POT:
       configura(PulsoTiristorConfig, map(analogRead(PinPot),0,1023,0,180 ) );           // Faz nova configuração dos valores
       lcd.setCursor(0, 0);         //Posiciona o cursor na coluna 3, linha 0; 
       lcd.print("ANGULO: ");         //Envia o texto entre aspas para o LCD
       lcd.setCursor(9, 0);         //Posiciona o cursor na coluna 3, linha 0; 
       switchBaixo10(map(analogRead(PinPot),0,1023,0,180));         
       break;

  case SetSubida:        
     lcd.setCursor(0, 0);         //Posiciona o cursor na coluna 3, linha 0; 
     lcd.print("TEMPO: ");         //Envia o texto entre aspas para o LCD 
     switchBaixo10(newPos); 
     lcd.setCursor(0, 1);         //Posiciona o cursor na coluna 3, linha 0; 
     lcd.print("CONFIGURADO: ");         //Envia o texto entre aspas para o LCD 
     switchBaixo10(TempoPraLigar);      
     break;    
    

 case Merda:        
     lcd.setCursor(3, 0);         //Posiciona o cursor na coluna 3, linha 0; 
     lcd.print("DEU MERDA: ");         //Envia o texto entre aspas para o LCD 
     lcd.setCursor(2, 1);         //Posiciona o cursor na coluna 3, linha 0; 
     lcd.print("REINICIANDO");         //Envia o texto entre aspas para o LCD     
     break;
     
   case SetDescida:        
     lcd.setCursor(0, 0);         //Posiciona o cursor na coluna 3, linha 0; 
     lcd.print("TEMPO: ");         //Envia o texto entre aspas para o LCD 
     switchBaixo10(newPos); 
     lcd.setCursor(0, 1);         //Posiciona o cursor na coluna 3, linha 0;  
     lcd.print("CONFIGURADO: ");         //Envia o texto entre aspas para o LCD 
     switchBaixo10(TempoPraDesligar);
     break;  

     case SetModo:        
       lcd.setCursor(0, 0);         //Posiciona o cursor na coluna 3, linha 0; 
       lcd.print("MODO: ");         //Envia o texto entre aspas para o LCD 
       if(newPosGlobal==PPM) lcd.print(" PPM ");         //Envia o texto entre aspas para o LCD
       if(newPosGlobal==PWM) lcd.print(" PWM ");         //Envia o texto entre aspas para o LCD  
       lcd.setCursor(0, 1);         //Posiciona o cursor na coluna 3, linha 0;  
       lcd.print("ATUAL: ");         //Envia o texto entre aspas para o LCD 
       if(MODO==PPM) lcd.print(" PPM ");         //Envia o texto entre aspas para o LCD
       if(MODO==PWM) lcd.print(" PWM ");         //Envia o texto entre aspas para o LCD 
       break;  
     
   case Subindo: 
     lcd.setCursor(0, 0);         //Posiciona o cursor na coluna 3, linha 0; 
     lcd.print("ANG DISPARO: ");         //Envia o texto entre aspas para o LCD
     if(AnguloDeDisparo > 0 ) lcd.print(AnguloDeDisparo);         //Envia o texto entre aspas para o LCD
     else  lcd.print("0");         //Envia o texto entre aspas para o LCD
     lcd.setCursor(0, 1);         //Posiciona o cursor na coluna 3, linha 0; 
     lcd.print("PROGRESSO: ");         //Envia o texto entre aspas para o LCD
     if(AnguloDeDisparo > 0 ) lcd.print( map(AnguloDeDisparo,180,0,0,100));         //Envia o texto entre aspas para o LCD
     else  lcd.print("100");         //Envia o texto entre aspas para o LCD
     lcd.print(" %");         //Envia o texto entre aspas para o LCD   
     break;  
       
   case Descendo:
     lcd.setCursor(0, 0);         //Posiciona o cursor na coluna 3, linha 0; 
     lcd.print("ANG DISPARO: ");         //Envia o texto entre aspas para o LCD
     lcd.print(AnguloDeDisparo);         //Envia o texto entre aspas para o LCD
     lcd.setCursor(0, 1);         //Posiciona o cursor na coluna 3, linha 0; 
     lcd.print("PROGRESSO: ");         //Envia o texto entre aspas para o LCD
     if(AnguloDeDisparo > 0 ) lcd.print( map(AnguloDeDisparo,0,180,0,100));         //Envia o texto entre aspas para o LCD
     else   lcd.print("100");         //Envia o texto entre aspas para o LCD
     lcd.print(" %");         //Envia o texto entre aspas para o LCD   
     break;   
 } 

 delay(100);    

 if(iniciaDown){  
    delay(1000);
    tela = Descendo;
    diminui=1; 
    Start = 0;
    AnguloDeDisparo = 0;
    FuncionandoNominal=0;
    atualizaTela=0; vai=1;
    iniciaDown = 0;  
    contadorTempo2 = contadorTempo = contadorTempo3 = contadorTempoDesliga = 0;
 }  
     
}
 

 



 








 



