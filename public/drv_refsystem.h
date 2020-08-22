#ifndef __DRV_REFSYSTEM_H
#define __DRV_REFSYSTEM_H
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#define    LEN_HEADER                         5        //֡ͷ����
#define    LEN_CMDID                          2        //�����볤��
#define    LEN_TAIL                           2	       //֡βCRC16

#define    LEN_game_state                     3	//0x0001
#define	   LEN_game_result       	          1	//0x0002
#define	   LEN_game_robot_survivors       	 32	//0x0003
#define    LEN_dart_status                    3 //0x0004
#define    LEN_ICRA_buff_status               3 //0x0005
#define	   LEN_event_data  					  4	//0x0101
#define	   LEN_supply_projectile_action       3	//0x0102
/*   #define	   LEN_supply_projectile_booking	  2	//0x0103   */
#define    LEN_referee_warning                2 //0x0104
#define    LEN_dart_remaining_time            1 //0x0105
#define	   LEN_game_robot_state    			 18	//0x0201
#define	   LEN_power_heat_data   			 16	//0x0202
#define	   LEN_game_robot_pos        		 16	//0x0203
#define	   LEN_buff_musk        			  1	//0x0204
#define	   LEN_aerial_robot_energy        	  3	//0x0205
#define	   LEN_robot_hurt        			  1	//0x0206
#define	   LEN_shoot_data       			  6	//0x0207
#define    LEN_bullet_remaining               2 //0x0208
#define    LEN_rfid_status                    4 //0x0209
#define    LEN_dart_client_cmd               12 //0x020A

#define    LEN_interact                       10 //
typedef enum
{
    FRAME_HEADER         = 0,
    CMD_ID               = 5,
    DATA                 = 7,

} FrameOffset_e;
typedef enum
{
    SOF          = 0,//����֡��ʼ�ֽ�
    DATA_LENGTH  = 1,//����֡��data�ĳ���
    SEQ          = 3,//�����
    CRC8         = 4 //CRC8

} FrameHeaderOffset_e;
typedef enum
{
    ID_game_state       			= 0x0001,//����״̬����
    ID_game_result 	   				= 0x0002,//�����������
    ID_game_robot_survivors       	= 0x0003,//����������Ѫ������
    ID_dart_status                  = 0x0004,//���ڷ���״̬
    ID_ICRA_buff_status             = 0x0005,//ICRA
    ID_event_data  					= 0x0101,//�����¼�����
    ID_supply_projectile_action   	= 0x0102,//���ز���վ������־����
//	ID_supply_projectile_booking 	= 0x0103,
    ID_referee_warning              = 0x0104,//���о�������
    ID_dart_remaining_time          = 0x0105,//���ڷ���ڵ���ʱ
    ID_game_robot_state    			= 0x0201,//������״̬����
    ID_power_heat_data    			= 0x0202,//ʵʱ������������
    ID_game_robot_pos        		= 0x0203,//������λ������
    ID_buff_musk					= 0x0204,//��������������
    ID_aerial_robot_energy			= 0x0205,//���л���������״̬����
    ID_robot_hurt					= 0x0206,//�˺�״̬����
    ID_shoot_data					= 0x0207,//ʵʱ�������
    ID_bullet_remaining             = 0x0208,//����ʣ�෢����
    ID_rfid_status                  = 0x0209,//������RFID״̬
    ID_dart_client_cmd              = 0x020A,//���ڻ����˿ͻ�������
    ID_InteractiveData              = 0x0301,//�����˼佻������
} CmdID_e;
//֡ͷ
typedef __packed struct
{
    uint8_t  SOF;//����֡��ʼ�ֽ�
    uint16_t DataLength;//����֡��data�ĳ���
    uint8_t  Seq;//�����
    uint8_t  CRC8;//CRC8У��

} FrameHeader_t;

/* ID: 0x0001  Byte:  3    ����״̬���� */
typedef __packed struct
{
    uint8_t game_type : 4;//��������
    uint8_t game_progress : 4;//��ǰ�����׶�
    uint16_t stage_remain_time;//��ǰ�׶�ʣ��ʱ��
} ext_game_state_t;


/* ID: 0x0002  Byte:  1    ����������� */
typedef __packed struct
{
    uint8_t winner;
} ext_game_result_t;


/* ID: 0x0003  28bits    ������Ѫ������ */
typedef __packed struct
{
    uint16_t red_1_robot_HP;
    uint16_t red_2_robot_HP;
    uint16_t red_3_robot_HP;
    uint16_t red_4_robot_HP;
    uint16_t red_5_robot_HP;
    uint16_t red_7_robot_HP;
    uint16_t red_outpost_HP;
    uint16_t red_base_HP;
    uint16_t blue_1_robot_HP;
    uint16_t blue_2_robot_HP;
    uint16_t blue_3_robot_HP;
    uint16_t blue_4_robot_HP;
    uint16_t blue_5_robot_HP;
    uint16_t blue_7_robot_HP;
    uint16_t blue_outpost_HP;
    uint16_t blue_base_HP;
} ext_game_robot_survivors_t;

/* ID: 0x0004  3bits  ���ڷ���״̬ */
typedef __packed struct
{
    uint8_t dart_belong;
    uint16_t stage_remaining_time;
} ext_dart_status_t;
/* ID: 0x0005 3bits ICRA*/
typedef __packed struct
{
    uint8_t F1_zone_status : 1;
    uint8_t F1_zone_buff_status : 3;
    uint8_t F2_zone_status : 1;
    uint8_t F2_zone_buff_status : 3;
    uint8_t F3_zone_status : 1;
    uint8_t F3_zone_buff_status : 3;
    uint8_t F4_zone_status : 1;
    uint8_t F4_zone_buff_status : 3;
    uint8_t F5_zone_status : 1;
    uint8_t F5_zone_buff_status : 3;
    uint8_t F6_zone_status : 1;
    uint8_t F6_zone_buff_status : 3;
} ext_ICRA_buff_status_t;
/* ID: 0x0101  Byte:  4    �����¼����� */
typedef __packed struct
{
    uint32_t event_type;
} ext_event_data_t;


/* ID: 0x0102  Byte:  4    ����վ������ʶ */
typedef __packed struct
{
    uint8_t supply_projectile_id;
    uint8_t supply_robot_id;
    uint8_t supply_projectile_step;
    uint8_t supply_projectile_num;
} ext_supply_projectile_action_t;


/* ID: 0X0103  Byte:  3
typedef __packed struct
{
	uint8_t supply_projectile_id;
    uint8_t supply_robot_id;
	uint8_t supply_num;
} ext_supply_projectile_booking_t; */

/* ID: 0X0104  Byte:  2    ���о������� */
typedef __packed struct
{
    uint8_t level;
    uint8_t foul_robot_id;
} ext_referee_warning_t;
/* ID: 0X0105  Byte:  1    ���ڷ���ڵ���ʱ */
typedef __packed struct
{
    uint8_t dart_remaining_time;
} ext_dart_remaining_time_t;
/* ID: 0X0201  Byte: 15    ����������״̬ */
typedef __packed struct
{
    uint8_t robot_id;   //������id �����ڽ�������У��
    uint8_t robot_level;  //�����˵ȼ�
    uint16_t remain_HP;  //ʣ��Ѫ��
    uint16_t max_HP; //Ѫ������
    uint16_t shooter_heat0_cooling_rate;
    uint16_t shooter_heat0_cooling_limit;
    uint16_t shooter_heat1_cooling_rate;
    uint16_t shooter_heat1_cooling_limit;
    uint8_t shooter_heat0_speed_limit;
    uint8_t shooter_heat1_speed_limit;
    uint8_t max_chassic_power;
    uint8_t mains_power_gimbal_output : 1;
    uint8_t mains_power_chassis_output : 1;
    uint8_t mains_power_shooter_output : 1;
} ext_game_robot_state_t;


/* ID: 0X0202  Byte: 14    ʵʱ������������ */
typedef __packed struct
{
    uint16_t chassis_volt;
    uint16_t chassis_current;
    float chassis_power;
    uint16_t chassis_power_buffer;
    uint16_t shooter_heat0;
    uint16_t shooter_heat1;
    uint16_t mobile_shooter_heat2;
} ext_power_heat_data_t;


/* ID: 0x0203  Byte: 16    ������λ������ */
typedef __packed struct
{
    float x;
    float y;
    float z;
    float yaw;
} ext_game_robot_pos_t;


/* ID: 0x0204  Byte:  1    �������������� */
typedef __packed struct
{
    uint8_t power_rune_buff;
} ext_buff_musk_t;


/* ID: 0x0205  Byte:  3    ���л���������״̬ */
typedef __packed struct
{
    uint8_t energy_point;
    uint8_t attack_time;
} aerial_robot_energy_t;


/* ID: 0x0206  Byte:  1    �˺�״̬ */
typedef __packed struct
{
    uint8_t armor_id : 4;
    uint8_t hurt_type : 4;
} ext_robot_hurt_t;


/* ID: 0x0207  Byte:  6    ʵʱ�����Ϣ */
typedef __packed struct
{
    uint8_t bullet_type;
    uint8_t bullet_freq;
    float bullet_speed;
} ext_shoot_data_t;
/* ID: 0x0208  Byte:  2    �ӵ�ʣ�෢���� */
typedef __packed struct
{
    uint16_t bullet_remaining_num;
} ext_bullet_remaining_t;
/* ID��0X0209  Byte:  4    ������RFID״̬ */
typedef __packed struct
{
    uint32_t rfid_status;
} ext_rfid_status_t;
/* ID: 0x020A  Byte:  12   ���ڻ����˿ͻ���ָ������ */
typedef __packed struct
{
    uint8_t dart_launch_opening_status;
    uint8_t dart_attack_target;
    uint16_t target_change_time;
    uint8_t first_dart_speed;
    uint8_t second_dart_speed;
    uint8_t third_dart_speed;
    uint8_t fourth_dart_speed;
    uint16_t last_dart_launch_time;
    uint16_t operate_launch_cmd_time;
} ext_dart_client_cmd_t;
/* ID: 0x0301  Byte:  n    �����˼佻������ */
typedef __packed struct
{
    uint16_t data_cmd_id;
    uint16_t send_ID;
    uint16_t receiver_ID;
} ext_student_interactive_header_data_t;
typedef __packed struct
{
    FrameHeader_t    FrameHeader;
    CmdID_e          CmdID;
    __packed union
    {
        ext_game_state_t       				  ext_game_state;					//0x0001
        ext_game_result_t            		  ext_game_result;					//0x0002
        ext_game_robot_survivors_t            ext_game_robot_survivors;			//0x0003
        ext_dart_status_t                     ext_dart_status;                  //0x0004
        ext_ICRA_buff_status_t                ext_ICRA_buff_status;             //0x0005
        ext_event_data_t        			  ext_event_data;					//0x0101
        ext_supply_projectile_action_t		  ext_supply_projectile_action;		//0x0102
        //ext_supply_projectile_booking_t		  ext_supply_projectile_booking;
        ext_referee_warning_t                 ext_referee_warning;              //0x0104
        ext_dart_remaining_time_t             ext_dart_remaining_time;          //0x0105
        ext_game_robot_state_t			  	  ext_game_robot_state;				//0x0201
        ext_power_heat_data_t		  		  ext_power_heat_data;				//0x0202
        ext_game_robot_pos_t				  ext_game_robot_pos;				//0x0203
        ext_buff_musk_t						  ext_buff_musk;					//0x0204
        aerial_robot_energy_t				  aerial_robot_energy;			    //0x0205
        ext_robot_hurt_t					  ext_robot_hurt;					//0x0206
        ext_shoot_data_t					  ext_shoot_data;					//0x0207
        ext_bullet_remaining_t                ext_bullet_remaining;             //0x0208
        ext_rfid_status_t                     ext_rfid_status;                  //0x0209
        ext_dart_client_cmd_t                 ext_dart_client_cmd;              //0x020A
        ext_student_interactive_header_data_t ext_interactiveData;              //0x0301
    } Data;
    uint16_t        CRC16;

} Frame_t;
typedef __packed struct
{
    FrameHeader_t    FrameHeader;
    CmdID_e          CmdID;
    ext_game_state_t       				  ext_game_state;					//0x0001
    ext_game_result_t            		  ext_game_result;					//0x0002
    ext_game_robot_survivors_t            ext_game_robot_survivors;			//0x0003
    ext_dart_status_t                     ext_dart_status;                  //0x0004
    ext_ICRA_buff_status_t                ext_ICRA_buff_status;             //0x0005
    ext_event_data_t        			  ext_event_data;					//0x0101
    ext_supply_projectile_action_t		  ext_supply_projectile_action;		//0x0102
    //ext_supply_projectile_booking_t		  ext_supply_projectile_booking;
    ext_referee_warning_t                 ext_referee_warning;              //0x0104
    ext_dart_remaining_time_t             ext_dart_remaining_time;          //0x0105
    ext_game_robot_state_t			  	  ext_game_robot_state;				//0x0201
    ext_power_heat_data_t		  		  ext_power_heat_data;				//0x0202
    ext_game_robot_pos_t				  ext_game_robot_pos;				//0x0203
    ext_buff_musk_t						  ext_buff_musk;					//0x0204
    aerial_robot_energy_t				  aerial_robot_energy;			    //0x0205
    ext_robot_hurt_t					  ext_robot_hurt;					//0x0206
    ext_shoot_data_t					  ext_shoot_data;					//0x0207
    ext_bullet_remaining_t                ext_bullet_remaining;             //0x0208
    ext_rfid_status_t                     ext_rfid_status;                  //0x0209
    ext_dart_client_cmd_t                 ext_dart_client_cmd;              //0x020A

    ext_student_interactive_header_data_t ext_interactiveData;               //0x0301
} DJI_Data_t;


typedef __packed struct
{
    float data1;
    float data2;
    float data3;
    uint8_t masks;
} client_custom_data_t;
typedef __packed struct
{
    FrameHeader_t   						txFrameHeader;//֡ͷ
    uint16_t		 						CmdID;
    ext_student_interactive_header_data_t   dataFrameHeader;
    client_custom_data_t  					clientData;
    uint16_t		 						FrameTail;
} ext_SendClientData_t;
typedef __packed struct
{
    uint8_t graphic_name[3];
    uint32_t operate_type : 3;
    uint32_t graphic_type : 3;
    uint32_t layer : 4;
    uint32_t color : 4;
    uint32_t start_angle : 9;
    uint32_t end_angle : 9;
    uint32_t width : 10;
    uint32_t start_x : 11;
    uint32_t start_y : 11;
    uint32_t radius : 10;
    uint32_t end_x : 11;
    uint32_t end_y : 11;
} graphic_data_struct_t;

typedef __packed struct
{
    uint8_t data[LEN_interact];
} robot_interactive_data_t;
typedef __packed struct
{
    FrameHeader_t   						txFrameHeader;
    uint16_t								CmdID;
    ext_student_interactive_header_data_t   dataFrameHeader;
    robot_interactive_data_t  	 			interactdata;
    uint16_t		 						FrameTail;
} ext_communatiandata_t;

typedef struct __Robodata
{
    uint8_t  robot_id;                          //������ID ����������У��
    uint8_t  robot_level;                       //�����˵ȼ�
    uint16_t remain_HP;                         //ʣ��Ѫ��
    uint16_t max_HP;                            //Ѫ������
    uint16_t cooling_rate_17;                   // 17mm ��ȴ���� ��λ /s
    uint16_t heat_limit_17;                     // 17mm ��������
    uint16_t cooling_rate_42;                   // 42mm ��ȴ���� ��λ /s
    uint16_t heat_limit_42;                     // 42mm ��������
    uint8_t  speed_limit_17;                    // 17mm �ӵ��ٶ�����
    uint8_t  speed_limit_42;                    // 42mm �ӵ��ٶ�����
    uint8_t  max_chassic_power;                 // ���̹�������
    uint8_t  mains_power_gimbal_output : 1;     // ��̨����״̬
    uint8_t  mains_power_chassis_output : 1;    // ���̹���״̬
    uint8_t  mains_power_shooter_output : 1;    // �����������״̬

    uint16_t chassis_volt;                      //���̵�ѹ
    uint16_t chassis_current;                   //���̵���
    float    chassis_power;                     //˲ʱ���̹���
    uint16_t chassis_power_buffer;              //60������������
    uint16_t heat_17;                           //17mmʵʱ����
    uint16_t heat_42;                           //42mmʵʱ����
    uint16_t heat_mobile;                       //����17mmʵʱ����
} Refdata_t;
extern Refdata_t *Refdata;
int DJI_Init(void);
#endif
