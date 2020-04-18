//
// MML�N���X���C�u���� V1.0
// �쐬  2019/03/24 by ���܋g����
// �C��  2019/05/23,�s��Ή�
// �C��  2020/01/19,M5Stack�ł̃R���p�C���G���[�Ή�
//

#include "MML.h"
//#define isBreak()  (false)

// note��`
const PROGMEM  uint16_t mml_scale[] = {
  4186,  // C
  4435,  // C#
  4699,  // D
  4978,  // D#
  5274,  // E
  5588,  // F
  5920,  // F#
  6272,  // G
  6643, // G#
  7040, // A
  7459, // A#
  7902, // B
};

// mml_scale�e�[�u���̃C���f�b�N�X
#define MML_C_BASE  0
#define MML_CS_BASE 1
#define MML_D_BASE  2
#define MML_DS_BASE 3
#define MML_E_BASE  4
#define MML_F_BASE  5
#define MML_FS_BASE 6
#define MML_G_BASE  7
#define MML_GS_BASE 8
#define MML_A_BASE  9
#define MML_AS_BASE 10
#define MML_B_BASE  11

const PROGMEM  uint8_t mml_scaleBase[] = {
  MML_A_BASE, MML_B_BASE, MML_C_BASE, MML_D_BASE, MML_E_BASE, MML_F_BASE, MML_G_BASE,
};

void MML::init() {
  playduration = 0;
  flgdbug = 0;      // �f�o�b�O�t���O
  if (func_init != 0)
    func_init();
}

// TONE ���g�� [,���o������ [,�{�����[��]]
// freq   ���g��
// tm     ���o������
// vol    ���o������
void MML::tone(int16_t freq, int16_t tm,int16_t vol) {
  if (func_tone != 0) 
    func_tone(freq, tm, vol);
}

//�@NOTONE
void MML::notone() {
  if (func_notone != 0)
    func_notone();
}

// 1�����o��
void  MML::debug(uint8_t c) {
  if (func_putc != 0)
    func_putc(c);
}

//  ���t�L���[�`�F�b�N
uint8_t MML::available() {
  if (!flgRun)
    return 0;
  if (playduration) {
    if ( endTick < millis() ) {
        if (!flgR) notone();
        playduration = 0;
        endTick = 0;
        return 1;
    } else {
      return 0;
    }
  }
  return 1;
}
    
// TEMPO �e���|
void MML::tempo(int16_t tempo) {
  if ( (tempo < MML_MIN_TMPO) || (tempo > MML_MAX_TMPO) ) 
    return;
  common_tempo = tempo;
}

// ���������̕]��
int16_t MML::getParamLen() {
  int16_t tmpLen = getParam();
  if (tmpLen == -1)
    tmpLen = 0;
  else if (!((tmpLen==1)||(tmpLen==2)||(tmpLen==4)||(tmpLen==8)||
          (tmpLen==16)||(tmpLen==32)||(tmpLen==64)) ) {
    // �����w��G���[
    tmpLen = -1;
    err = ERR_MML; 
  }
  return tmpLen;
}

// �����̕]��
//  �߂�l
//   �����Ȃ� -1 �������� 0�ȏ�
int16_t MML::getParam() {
  int16_t tmpParam = 0;
  char* tmpPtr = mml_ptr;
  while(isdigit(*mml_ptr)) {
     if (flgdbug) debug(*mml_ptr); // �f�o�b�O
     tmpParam*= 10;
     tmpParam+= *mml_ptr - '0';
     mml_ptr++;
  }
  if (tmpPtr == mml_ptr) {
    tmpParam = -1;
  }
  return tmpParam;
}

// ���t�J�n
// ����
//  mode  0�r�b�g  0:�t�H�A�O�����h���t�A1:�o�b�N�O���E���h���t
//        1�r�b�g  0:�擪����            1:���f�r������
//        2�r�b�g  0:���s�[�g�Ȃ�        1:���s�[�g���� (�o�b�N�O���E���h���t���̂݁j
void MML::play(uint8_t mode) {  
  if (!(mode & MML_RESUME))  mml_ptr = mml_text;              // �擪����̉��t
  repeat =  ((mode & MML_REPEAT) && (mode & MML_BGM)) ? 1:0;  // ���s�[�g���[�h
  if ( !(mode & MML_BGM) ) {
    playMode = 1;
    flgRun = 1;
    playTick(0);  // �t�H�A�O�����h���t
  } else {
    flgRun = 1;
    playMode = 2; // �o�b�N�O�����h���t
  }
}

// PLAY ������
void MML::playTick(uint8_t flgTick) {
  uint16_t freq;                     // ���g��
  uint16_t local_len = common_len ;  // �ʒ���
  uint8_t  local_oct = common_oct ;  // �ʍ���
  
  int8_t  scale = 0;                 // ���K
  uint32_t duration;                 // �Đ�����(msec)
  uint8_t flgExtlen = 0;
  uint8_t c;                         // 1�������o���p
  int16_t tmpLen;                    // ���̒����]���p
  err = 0; 

  // MML�̕]��
  while(*mml_ptr) {
    if (flgdbug) debug(*mml_ptr); // �f�o�b�O
    flgExtlen = 0;
    local_len = common_len;
    local_oct = common_oct;

    // ���f�̔���
    if (!flgRun) {
      break;
    }
  
    // �󔒂̓X�L�b�v    
    if (*mml_ptr == ' '|| *mml_ptr == '&') {
      mml_ptr++;
      continue;
    }

    // �f�o�b�O�R�}���h
    if (*mml_ptr == '?') {
      if (flgdbug) 
        flgdbug = 0;
      else 
        flgdbug = 1;
      mml_ptr++;
      continue;
    }   

     c = toUpperCase(*mml_ptr);
    if ( ((c >= 'A') && (c <= 'G')) || (c == 'R') ) {  //**** ���K�L�� A - Z,R 
      flgR = 0;
      if (c == 'R') {
        flgR = 1;
        mml_ptr++;
      } else {
        scale = pgm_read_byte(mml_scaleBase + c-'A'); // ���K�R�[�h�̎擾   
        mml_ptr++;
        if (*mml_ptr == '#' || *mml_ptr == '+') {
          //** �ʂ̉��K�����グ # or +
          if (flgdbug) debug(*mml_ptr); // �f�o�b�O
          // �����グ��
          if (scale < MML_B_BASE) {
            scale++;
          } else {
            if (local_oct < MML_MAX_OCT) {
              scale = MML_B_BASE;
              local_oct++;
            }
          }
          mml_ptr++;        
        } else if (*mml_ptr == '-') {
          //** �ʂ̉��K�������� # or +
          if (flgdbug) debug(*mml_ptr); // �f�o�b�O
          // ����������
          if (scale > MML_C_BASE) {
            scale--;
          } else {
            if (local_oct > 1) {
              scale = MML_B_BASE;
              local_oct--;
            }
          }                
          mml_ptr++;      
        } 
      }
      
      //** �ʂ̒����̎w�� 
      if ( (tmpLen = getParamLen() ) < 0)
        break;
      if (tmpLen > 0) {
        local_len = tmpLen;
      }
      
      //** �����L�΂�
      if (*mml_ptr == '.') {
        if (flgdbug) debug(*mml_ptr); // �f�o�b�O
        mml_ptr++;
        flgExtlen = 1;
      } 

      //** ���K�̍Đ�
      duration = 240000/common_tempo/local_len;  // �Đ�����(msec)
      if (flgExtlen)duration += duration>>1;

      if (flgR) {
        // �x��
        if (flgTick) {
          playduration = duration;
          endTick = millis()+duration;
          break;
        } else {
          delay(duration); 
        }
      } else {
        // ����
        freq = pgm_read_word(&mml_scale[scale])>>(MML_MAX_OCT-local_oct); // �Đ����g��(Hz);  
        if (flgTick) {         
          playduration = duration;
          endTick = millis()+duration;
          tone(freq, 0, common_vol);                      // ���̍Đ�(���Ԏw��Ȃ��j
          break;
        } else {
          tone(freq, (uint16_t)duration, common_vol);     // ���̍Đ�   
        }
      }
      
    } else if (c == 'L') {  // �O���[�o���Ȓ����̎w��     
      //**** �ȗ��������w�� L[n][.] 
      mml_ptr++;
       if ( (tmpLen = getParamLen() ) < 0)
        break;
      if (tmpLen > 0) {
        local_len = tmpLen;
        common_len = tmpLen;
        
        //** �����L�΂�
        if (*mml_ptr == '.') {
           if (flgdbug) debug(*mml_ptr); // �f�o�b�O
            mml_ptr++;
            common_len += common_len>>1;
            local_len =  common_len;
        } 
      } else {
        // �����ȗ����́A�f�t�H���g��ݒ肷��
        common_len = MML_len;
        local_len =  MML_len;              
      }
    //**** �{�����[���w�� Vn 
    } else if (c == 'V') {  // �O���[�o���ȃ{�����[���̎w��     
      mml_ptr++;
      uint16_t tmpVol = getParam();
      if (tmpVol < 0 || tmpVol > MML_MAX_VOL) {
        err = ERR_MML; 
        break;
      }
      common_vol = tmpVol;     
    //**** ���̍����w�� On 
    } else if (c == 'O') { // �O���[�o���ȃI�N�^�[�u�̎w��
      mml_ptr++;
      uint16_t tmpOct = getParam();
      if (tmpOct < 1 || tmpOct > MML_MAX_OCT) {
        err = ERR_MML; 
        break;
      }
      common_oct = tmpOct;
      local_oct = tmpOct;
    } else if (c == '>') { // �O���[�o����1�I�N�^�[�u�グ��
      if (common_oct < MML_MAX_OCT) {
        common_oct++;
      }
      mml_ptr++;
    //**** 1�I�N�^�[�u������ < 
    } else if (c == '<') { // �O���[�o����1�I�N�^�[�u������
      if (common_oct > 1) {
        common_oct--;
      }
      mml_ptr++;
    //**** �e���|�w�� Tn 
    } else if (c == 'T') { // �O���[�o���ȃe���|�̎w��
      mml_ptr++;      
      //** �����̎w��
      uint32_t tmpTempo = getParam();
      if (tmpTempo < MML_MIN_TMPO || tmpTempo > MML_MAX_TMPO) {
        err = ERR_MML; 
        break;               
      }
      common_tempo = tmpTempo;      
    } else {
      err = ERR_MML; 
      break;
    }
  }
  if ((!*mml_ptr && available()) || isError() ) {
    flgRun = 0;    // ���t�I��
    playMode = 0;
  }
}