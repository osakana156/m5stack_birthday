//
// MML�N���X���C�u���� V1.0
// �쐬�� 2019/03/24 by ���܋g����
// �ŏI�X�V�� 2019/05/23 by ���܋g����
//

#ifndef ___MML_h___
#define ___MML_h___

#include "Arduino.h"

// �G���[���b�Z�[�W��`
#define ERR_MML 1

// �f�t�H���g�ݒ�l
#define MML_tempo     120 // �e���|(32�`255)
#define MML_len       4   // ����(1,2,4,8,16,32)
#define MML_oct       4   // ���̍���(1�`8)
#define MML_vol       15  // ���̑傫��(1�`15)

// �����͈�
#define MML_MAX_VOL  15   // ���̑傫��(1�`15)
#define MML_MAX_OCT  8    // ���̍���(1�`8)
#define MML_MAX_TMPO 255  //  �e���|(32�`255)
#define MML_MIN_TMPO 32   //  �e���|(32�`255)

// ���t���[�h(play()�̈���)
#define MML_BGM         0b001  // �o�b�N�O���E���h���t
#define MML_RESUME      0b010  // ���f�ʒu���牉�t
#define MML_REPEAT      0b100  // �J��Ԃ����t

class MML {
  private:
    uint8_t   err = 0;                      // Error message index
    uint16_t  common_tempo = MML_tempo ;    // ���ʃe���|
    uint16_t  common_len   = MML_len   ;    // ���ʒ���
    uint8_t   common_oct   = MML_oct   ;    // ���ʍ���
    uint8_t   common_vol   = MML_vol   ;    // ���̑傫��
    uint8_t   flgdbug = 0;                  // �f�o�b�O�o�̓t���O

    char      *mml_text = 0;                // MML���e�L�X�g
    char      *mml_ptr = 0;                 // �Đ��ʒu
    uint16_t  playduration = 0;             // �������̉����E�x���̒����i�~���b�j
    uint8_t   flgR = 0;                     // �x�����t�t���O
    uint32_t  endTick = 0;                  // �Đ��I���V�X�e�����ԁi�~���b�j
    uint32_t  repeat  = 0;                  // �J��Ԃ����t
    volatile uint8_t   flgRun;              // ���s�����
    volatile uint8_t   playMode = 0;        // ���t��� 0:��~ 1:�t�H�A�O�����h���t 2:�o�b�N�O���E���h���t

    void (*func_tone)(uint16_t freq, uint16_t tm, uint16_t vol) = 0;
    void (*func_notone)(void) = 0;
    void (*func_init)(void) = 0;
    void (*func_putc)(uint8_t c) = 0;
  
  private:
	  int16_t getParamLen();
    int16_t getParam();
    void tone(int16_t freq, int16_t tm = 0,int16_t vol=15);
    void notone();
    void debug(uint8_t c);
    void init();   

  public:
    // �n�[�h�E�F�A�ˑ��֐��̃A�^�b�`
 
    void init(
       void (*f1)(void), 
       void (*f2)(uint16_t freq, uint16_t tm, uint16_t vol),
       void (*f3)(void), 
       void (*f4)(uint8_t c) = 0    ) 
    { func_init = f1; func_tone = f2; func_notone = f3; func_putc = f4; init(); }; 


    void setText(const char* text)  // MML���̓o�^
    { mml_text = (char *)text; };

    void tempo(int16_t tempo);

    void playTick(uint8_t flgTick = true);
    void play(uint8_t mode = 0);
    void playBGM(uint8_t mode = 0) {play(MML_BGM|mode);};
    uint8_t isBGMPlay()  {return ((playMode == 2) && flgRun); };
    uint8_t isPlay()  {return ((playMode == 1) && flgRun); };
    uint8_t available(); 
    uint8_t isError()  {return err; };
    void stop() { notone(); flgRun = 0;};
    void resume() {if (*mml_text) flgRun = 1; };
};

#endif