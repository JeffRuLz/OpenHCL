#include "text.hpp"
#include "assets.hpp"
#include "system.hpp"
#include "game.hpp"
#include <stdio.h>
#include <string.h>

typedef struct
{
	unsigned short character[MAX_CHARACTERS];
	unsigned char length;
} Message;

#define MAX_CHARACTERS 48
#define TABLE_SIZE (20*24)

static Message text[TEXT_LAST];
static bool forcePlain = true;

static char label[][8] = {
	"jp",
	"en",
	"kr",
	"na"
};

#define utf8(X) (((X[0] & 0xFF) << 16) | ((X[1] & 0xFF) << 8) | (X[2] & 0xFF))

static unsigned int characterTable[TABLE_SIZE] = {
	' ', 0, utf8("。"), ',', '.', 0, ':', ';', '?', '!', '"', 0, '\'', utf8("︑"), 0, '^', 0, '_', 0, 0, 
	0, 0, 0, 0, 0, 0, 0, '-', 0, 0, '/', '\\', '~', 0, '|', utf8("…"), 0, 0, 0, 0, 
	0, '(', ')', 0, 0, 0, 0, '{', '}', '<', '>', 0, 0, 0, 0, 0, 0, '[', ']', '+', 
	0, 0, 0, 0, '=', 0, 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0, 0, '$', 
	0, 0, '%', '#', '&', '*', '@', 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 0, 0, 0, 0, 0, 0, 0, 
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
	'U', 'V', 'W', 'X', 'Y', 'Z', 0, 0, 0, 0, 0, 0, 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
	'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 0, 0,
	0, 0, utf8("ぁ"), utf8("あ"), utf8("ぃ"), utf8("い"), utf8("ぅ"), utf8("う"), utf8("ぇ"), utf8("え"), utf8("ぉ"), utf8("お"), utf8("か"), utf8("が"), utf8("き"), utf8("ぎ"), utf8("く"), utf8("ぐ"), utf8("け"), utf8("げ"),
	utf8("こ"), utf8("ご"), utf8("さ"), utf8("ざ"), utf8("し"), utf8("じ"), utf8("す"), utf8("ず"), utf8("せ"), utf8("ぜ"), utf8("そ"), utf8("ぞ"), utf8("た"), utf8("だ"), utf8("ち"), utf8("ぢ"), utf8("っ"), utf8("つ"), utf8("づ"), utf8("て"),
	utf8("で"), utf8("と"), utf8("ど"), utf8("な"), utf8("に"), utf8("ぬ"), utf8("ね"), utf8("の"), utf8("は"), utf8("ば"), utf8("ぱ"), utf8("ひ"), utf8("び"), utf8("ぴ"), utf8("ふ"), utf8("ぶ"), utf8("ぷ"), utf8("へ"), utf8("べ"), utf8("ぺ"),
	utf8("ほ"), utf8("ぼ"), utf8("ぽ"), utf8("ま"), utf8("み"), utf8("む"), utf8("め"), utf8("も"), utf8("ゃ"), utf8("や"), utf8("ゅ"), utf8("ゆ"), utf8("ょ"), utf8("よ"), utf8("ら"), utf8("り"), utf8("る"), utf8("れ"), utf8("ろ"), utf8("ゎ"), 
	utf8("わ"), utf8("ゐ"), utf8("ゑ"), utf8("を"), utf8("ん"), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, utf8("ァ"), utf8("ア"), utf8("ィ"), utf8("イ"),
	utf8("ゥ"), utf8("ウ"), utf8("ェ"), utf8("エ"), utf8("ォ"), utf8("オ"), utf8("カ"), utf8("ガ"), utf8("キ"), utf8("ギ"), utf8("ク"), utf8("グ"), utf8("ケ"), utf8("ゲ"), utf8("コ"), utf8("ゴ"), utf8("サ"), utf8("ザ"), utf8("シ"), utf8("ジ"), 
	utf8("ス"), utf8("ズ"), utf8("セ"), utf8("ゼ"), utf8("ソ"), utf8("ゾ"), utf8("タ"), utf8("ダ"), utf8("チ"), utf8("ヂ"), utf8("ッ"), utf8("ツ"), utf8("ヅ"), utf8("テ"), utf8("デ"), utf8("ト"), utf8("ド"), utf8("ナ"), utf8("ニ"), utf8("ヌ"), 
	utf8("ネ"), utf8("ノ"), utf8("ハ"), utf8("バ"), utf8("パ"), utf8("ヒ"), utf8("ビ"), utf8("ピ"), utf8("フ"), utf8("ブ"), utf8("プ"), utf8("ヘ"), utf8("ベ"), utf8("ペ"), utf8("ホ"), utf8("ボ"), utf8("ポ"), utf8("マ"), utf8("ミ"), utf8("ム"),	
	utf8("メ"), utf8("モ"), utf8("ャ"), utf8("ヤ"), utf8("ュ"), utf8("ユ"), utf8("ョ"), utf8("ヨ"), utf8("ラ"), utf8("リ"), utf8("ル"), utf8("レ"), utf8("ロ"), utf8("ヮ"), utf8("ワ"), utf8("ヰ"), utf8("ヱ"), utf8("ヲ"), utf8("ン"), utf8("ヴ"), 
	utf8("ヵ"), utf8("ヶ"), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static unsigned int koreanTable[50*50] = {
	utf8("가"), utf8("각"), utf8("간"), utf8("갇"), utf8("갈"), utf8("갉"), utf8("갊"), utf8("감"), utf8("갑"), utf8("값"), utf8("갓"), utf8("갔"), utf8("강"), utf8("갖"), utf8("갗"), utf8("같"), utf8("갚"), utf8("갛"), utf8("개"), utf8("객"), 	
	utf8("갠"), 	utf8("갤"), utf8("갬"), utf8("갭"), utf8("갯"), utf8("갰"), utf8("갱"), utf8("갸"), utf8("갹"), utf8("갼"), utf8("걀"), utf8("걋"), utf8("걍"), utf8("걔"), utf8("걘"), utf8("걜"), utf8("거"), utf8("걱"), utf8("건"), utf8("걷"), 
	utf8("걸"), 	utf8("걺"), utf8("검"), utf8("겁"), utf8("것"), utf8("겄"), utf8("겅"), utf8("겆"), utf8("겉"), utf8("겊"), 

	utf8("겋"), utf8("게"), utf8("겐"), utf8("겔"), utf8("겜"), utf8("겝"), utf8("겟"), utf8("겠"), utf8("겡"), utf8("겨"), utf8("격"), utf8("겪"), utf8("견"), utf8("겯"), utf8("결"), utf8("겸"), utf8("겹"), utf8("겻"), utf8("겼"), utf8("경"), 
	utf8("곁"), utf8("계"), utf8("곈"), utf8("곌"), utf8("곕"), utf8("곗"), utf8("고"), utf8("곡"), utf8("곤"), utf8("곧"), utf8("골"), utf8("곪"), utf8("곬"), utf8("곯"), utf8("곰"), utf8("곱"), utf8("곳"), utf8("공"), utf8("곶"), utf8("과"), 
	utf8("곽"), utf8("관"), utf8("괄"), utf8("괆"), utf8("괌"), utf8("괍"), utf8("괏"), utf8("광"), utf8("괘"), utf8("괜"), 

	utf8("괠"), utf8("괩"), utf8("괬"), utf8("괭"), utf8("괴"), utf8("괵"), utf8("괸"), utf8("괼"), utf8("굄"), utf8("굅"), utf8("굇"), utf8("굉"), utf8("교"), utf8("굔"), utf8("굘"), utf8("굡"), utf8("굣"), utf8("구"), utf8("국"), utf8("군"), 
	utf8("굳"), utf8("굴"), utf8("굵"), utf8("굶"), utf8("굻"), utf8("굼"), utf8("굽"), utf8("굿"), utf8("궁"), utf8("궃"), utf8("궈"), utf8("궉"), utf8("권"), utf8("궐"), utf8("궜"), utf8("궝"), utf8("궤"), utf8("궷"), utf8("귀"), utf8("귁"), 
	utf8("귄"), utf8("귈"), utf8("귐"), utf8("귑"), utf8("귓"), utf8("규"), utf8("균"), utf8("귤"), utf8("그"), utf8("극"),

	utf8("근"), utf8("귿"), utf8("글"), utf8("긁"), utf8("금"), utf8("급"), utf8("긋"), utf8("긍"), utf8("긔"), utf8("기"), utf8("긱"), utf8("긴"), utf8("긷"), utf8("길"), utf8("긺"), utf8("김"), utf8("깁"), utf8("깃"), utf8("깅"), utf8("깆"), 
	utf8("깊"), utf8("까"), utf8("깍"), utf8("깎"), utf8("깐"), utf8("깔"), utf8("깖"), utf8("깜"), utf8("깝"), utf8("깟"), utf8("깠"), utf8("깡"), utf8("깥"), utf8("깨"), utf8("깩"), utf8("깬"), utf8("깰"), utf8("깸"), utf8("깹"), utf8("깻"), 
	utf8("깼"), utf8("깽"), utf8("꺄"), utf8("꺅"), utf8("꺌"), utf8("꺼"), utf8("꺽"), utf8("꺾"), utf8("껀"), utf8("껄"), 

	utf8("껌"), utf8("껍"), utf8("껏"), utf8("껐"), utf8("껑"), utf8("께"), utf8("껙"), utf8("껜"), utf8("껨"), utf8("껫"), utf8("껭"), utf8("껴"), utf8("껸"), utf8("껼"), utf8("꼇"), utf8("꼈"), utf8("꼍"), utf8("꼐"), utf8("꼬"), utf8("꼭"), 
	utf8("꼰"), utf8("꼲"), utf8("꼴"), utf8("꼼"), utf8("꼽"), utf8("꼿"), utf8("꽁"), utf8("꽂"), utf8("꽃"), utf8("꽈"), utf8("꽉"), utf8("꽐"), utf8("꽜"), utf8("꽝"), utf8("꽤"), utf8("꽥"), utf8("꽹"), utf8("꾀"), utf8("꾄"), utf8("꾈"), 
	utf8("꾐"), utf8("꾑"), utf8("꾕"), utf8("꾜"), utf8("꾸"), utf8("꾹"), utf8("꾼"), utf8("꿀"), utf8("꿇"), utf8("꿈"),

	utf8("꿉"), utf8("꿋"), utf8("꿍"), utf8("꿎"), utf8("꿔"), utf8("꿜"), utf8("꿨"), utf8("꿩"), utf8("꿰"), utf8("꿱"), utf8("꿴"), utf8("꿸"), utf8("뀀"), utf8("뀁"), utf8("뀄"), utf8("뀌"), utf8("뀐"), utf8("뀔"), utf8("뀜"), utf8("뀝"), 
	utf8("뀨"), utf8("끄"), utf8("끅"), utf8("끈"), utf8("끊"), utf8("끌"), utf8("끎"), utf8("끓"), utf8("끔"), utf8("끕"), utf8("끗"), utf8("끙"), utf8("끝"), utf8("끼"), utf8("끽"), utf8("낀"), utf8("낄"), utf8("낌"), utf8("낍"), utf8("낏"), 
	utf8("낑"), utf8("나"), utf8("낙"), utf8("낚"), utf8("난"), utf8("낟"), utf8("날"), utf8("낡"), utf8("낢"), utf8("남"), 

	utf8("납"), utf8("낫"), utf8("났"), utf8("낭"), utf8("낮"), utf8("낯"), utf8("낱"), utf8("낳"), utf8("내"), utf8("낵"), utf8("낸"), utf8("낼"), utf8("냄"), utf8("냅"), utf8("냇"), utf8("냈"), utf8("냉"), utf8("냐"), utf8("냑"), utf8("냔"), 
	utf8("냘"), utf8("냠"), utf8("냥"), utf8("너"), utf8("넉"), utf8("넋"), utf8("넌"), utf8("널"), utf8("넒"), utf8("넓"), utf8("넘"), utf8("넙"), utf8("넛"), utf8("넜"), utf8("넝"), utf8("넣"), utf8("네"), utf8("넥"), utf8("넨"), utf8("넬"), 
	utf8("넴"), utf8("넵"), utf8("넷"), utf8("넸"), utf8("넹"), utf8("녀"), utf8("녁"), utf8("년"), utf8("녈"), utf8("념"), 

	utf8("녑"), utf8("녔"), utf8("녕"), utf8("녘"), utf8("녜"), utf8("녠"), utf8("노"), utf8("녹"), utf8("논"), utf8("놀"), utf8("놂"), utf8("놈"), utf8("놉"), utf8("놋"), utf8("농"), utf8("높"), utf8("놓"), utf8("놔"), utf8("놘"), utf8("놜"), 
	utf8("놨"), utf8("뇌"), utf8("뇐"), utf8("뇔"), utf8("뇜"), utf8("뇝"), utf8("뇟"), utf8("뇨"), utf8("뇩"), utf8("뇬"), utf8("뇰"), utf8("뇹"), utf8("뇻"), utf8("뇽"), utf8("누"), utf8("눅"), utf8("눈"), utf8("눋"), utf8("눌"), utf8("눔"), 
	utf8("눕"), utf8("눗"), utf8("눙"), utf8("눠"), utf8("눴"), utf8("눼"), utf8("뉘"), utf8("뉜"), utf8("뉠"), utf8("뉨"),

	utf8("뉩"),0,0,0,0,0,0,utf8("느"),0,0, 0,0,0,0,0,0,0,0,utf8("늪"),0, 0,0,utf8("니"),0,0,0,0,0,0,0, 0,0,utf8("다"),0,0,utf8("단"),0,0,0,0, 0,0,0,0,0,0,0,0,0,0,
	utf8("대"),0,0,0,0,0,0,0,0,0, utf8("더"),0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, utf8("도"),utf8("독"),0,0,utf8("돌"),0,0,0,0,0,
	utf8("동"),0,0,0,0,0,0,0,0,0, 0,0,0,0,0,utf8("두"),0,0,0,utf8("둠"), 0,0,utf8("둥"),0,0,0,0,utf8("뒤"),0,0, 0,0,0,0,0,0,0,0,utf8("드"),0, utf8("든"),0,0,0,0,0,0,0,0,utf8("디"),
	utf8("딕"),0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, utf8("때"),0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,
	utf8("뗘"),0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,utf8("라"),utf8("락"),utf8("란"),0,
	utf8("람"),0,0,0,utf8("랑"),0,0,0,0,0, 0,0,utf8("램"),0,0,0,0,0,0,0, 0,0,utf8("러"),0,0,0,0,0,0,0, 0,0,utf8("레"),0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,
	utf8("롄"),0,0,utf8("로"),utf8("록"),0,utf8("롤"),0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,utf8("료"),0,0,0,0,0,utf8("루"), 0,0,0,utf8("룸"),0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,
	utf8("률"),0,0,0,0,utf8("르"),0,utf8("른"),utf8("를"),0, 0,0,0,0,0,0,utf8("리"),0,0,0, utf8("림"),0,utf8("릿"),0,utf8("마"),0,0,0,0,0, 0,0,0,0,0,utf8("망"),0,0,0,0, 0,0,0,0,0,0,0,0,0,0,
	utf8("먁"),0,0,utf8("머"),0,0,0,0,0,0, 0,0,0,0,utf8("메"),0,0,0,0,0, 0,0,0,0,0,utf8("면"),0,0,0,0, 0,0,utf8("모"),0,0,0,utf8("몰"),0,0,0, 0,0,0,0,0,0,0,0,0,0,
	utf8("묏"),0,0,0,0,0,0,utf8("무"),0,0, utf8("문"),0,utf8("물"),0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, utf8("미"),0,0,0,0,0,0,0,0,0,
	utf8("밍"),0,0,utf8("바"),utf8("박"),0,0,utf8("반"),0,utf8("발"), utf8("밝"),0,0,0,0,0,utf8("방"),0,0,0, utf8("밴"),0,utf8("뱀"),0,0,0,0,0,0,0, 0,0,0,0,utf8("번"),0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,
	utf8("벳"),0,0,0,0,0,utf8("별"),0,0,0, 0,0,0,0,utf8("보"),0,0,0,utf8("볼"),0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,utf8("부"),0,0,0, utf8("불"),0,0,0,0,0,0,0,0,0,
	utf8("붤"),0,0,0,0,0,0,0,0,0, 0,0,0,0,utf8("브"),0,0,utf8("블"),0,0, 0,utf8("비"),0,0,0,0,0,0,0,0, 0,utf8("빛"),utf8("빠"),0,0,utf8("빨"),0,0,0,0, 0,0,0,0,0,0,0,0,0,0,
	utf8("뺐"),0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,
	utf8("삘"),0,0,0,0,utf8("사"),0,0,0,0, 0,0,0,0,0,0,0,utf8("상"),0,utf8("새"), utf8("색"),0,0,0,0,0,0,utf8("생"),0,0, 0,0,0,0,0,0,0,0,0,0, 0,utf8("서"),utf8("석"),0,0,0,0,0,0,0,
	utf8("섬"),0,0,0,0,0,0,0,0,0, 0,0,utf8("셋"),0,0,0,0,utf8("션"),0,0, 0,0,0,0,0,0,0,0,utf8("소"),utf8("속"), 0,0,0,0,0,0,0,utf8("송"),0,0, 0,0,0,0,0,0,0,0,0,0,
	utf8("쇠"),0,0,0,0,0,0,0,0,0, 0,0,0,0,utf8("수"),0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,utf8("스"),0,
	utf8("슨"),0,0,0,utf8("습"),0,utf8("승"),utf8("시"),0,0, 0,utf8("실"),0,0,utf8("십"),0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,
	utf8("쎄"),0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,
	utf8("씩"),0,0,0,0,0,0,utf8("아"),utf8("악"),0, 0,0,utf8("알"),0,0,0,0,0,0,utf8("았"), 0,0,0,0,0,0,0,0,0,0, 0,0,utf8("야"),0,0,0,0,0,0,0, utf8("양"),0,0,0,0,0,0,utf8("어"),0,utf8("언"),
	utf8("얹"),0,0,0,0,0,utf8("업"),0,0,0, 0,0,0,0,utf8("에"),0,0,0,0,0, 0,0,utf8("여"),utf8("역"),0,0,utf8("열"),0,0,0, utf8("엽"),0,0,0,utf8("영"),0,0,0,0,0, 0,0,0,0,0,utf8("오"),0,0,utf8("올"),0,
	utf8("옮"),0,0,0,utf8("옵"),0,0,0,0,0, 0,0,0,0,0,0,utf8("왕"),0,0,0, 0,0,0,0,0,utf8("왼"),0,0,0,0, 0,0,0,0,0,0,0,0,utf8("용"),0, 0,utf8("운"),utf8("울"),0,0,utf8("움"),0,0,0,utf8("워"),
	utf8("웍"),0,0,0,0,0,0,0,0,0, 0,0,0,0,utf8("위"),0,0,0,0,0, 0,0,utf8("유"),0,0,0,0,0,0,0, 0,0,0,utf8("은"),utf8("을"),0,utf8("음"),0,0,0, 0,0,0,0,0,0,utf8("의"),0,0,0,
	utf8("읫"),utf8("이"),utf8("익"),utf8("인"),0,0,0,0,utf8("임"),0, 0,utf8("있"),0,0,0,utf8("자"),0,0,0,0, 0,0,0,utf8("잡"),0,0,utf8("장"),0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,utf8("저"),utf8("적"),utf8("전"),
	utf8("절"),0,utf8("점"),0,0,utf8("정"),0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,utf8("조"),0,0,0,0,0,0, 0,utf8("종"),0,0,0,utf8("좌"),0,0,0,0, 0,0,0,0,utf8("죄"),0,0,0,0,0,
	utf8("죙"),0,0,0,0,0,0,utf8("준"),0,0, 0,0,0,0,utf8("중"),0,0,0,utf8("쥐"),0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,utf8("즘"),0,0,0,utf8("지"),utf8("직"),utf8("진"), 0,0,0,0,0,0,0,0,0,0,
	utf8("짜"),0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,utf8("쪽"),0,0, 0,0,0,0,0,0,0,0,0,0,
	utf8("쬈"),0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,utf8("차"),0,0,0,0,0,0, 0,0,0,utf8("찾"),0,0,0,0,0,0,
	utf8("챗"),0,0,0,0,0,0,0,0,utf8("처"), 0,0,0,0,0,0,0,0,utf8("체"),0, 0,0,0,0,0,0,0,0,0,0, 0,0,utf8("초"),0,0,0,0,0,0,utf8("총"), 0,0,0,0,0,0,0,0,0,0,
	utf8("쵷"),0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,utf8("츠"),0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,
	utf8("캅"),0,0,0,0,0,0,0,0,0, 0,0,0,0,0,utf8("커"),0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,utf8("켬"),0,0,0, 0,0,0,0,0,0,0,0,0,0,
	utf8("콰"),0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,utf8("크"),0,0, utf8("클"),0,0,0,utf8("키"),0,0,0,0,0,
	utf8("킷"),0,utf8("타"),0,utf8("탄"),0,0,0,0,0, 0,0,utf8("태"),0,0,0,0,0,0,0, 0,0,0,0,0,0,utf8("털"),0,0,0, 0,0,0,utf8("테"),0,0,0,utf8("템"),0,0, 0,0,0,0,0,0,utf8("토"),0,0,0,
	utf8("톰"),0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,utf8("트"),utf8("특"),0,0, 0,0,0,0,0,0,0,0,0,0,
	utf8("티"),0,0,0,0,0,0,0,utf8("파"),0, 0,0,0,0,0,0,0,0,0,0, utf8("패"),0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,
	utf8("펄"),0,0,0,0,0,0,0,0,0, utf8("폭"),0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,utf8("프"),
	utf8("픈"),utf8("플"),0,0,0,0,0,0,0,0, 0,0,0,utf8("하"),0,utf8("한"),0,0,0,utf8("합"), 0,utf8("항"),0,0,0,0,0,0,0,utf8("했"), 0,0,utf8("향"),utf8("허"),0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,
	utf8("혀"),0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,utf8("화"),utf8("확"),0,0,0,utf8("황"),0, 0,0,0,0,0,0,0,0,0,0, 0,utf8("효"),0,0,0,0,0,0,0,0,
	utf8("훑"),0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,utf8("히"),0,0,
	utf8("힐"),utf8("힘"),utf8("힙"),utf8("힛"),utf8("힝")
};

/*
static void _decomposeHangul(unsigned int in, unsigned int* L, unsigned int* V, unsigned int* T)
{
	//crunch utf-32 into utf-16
	unsigned short val = ((in & 0xF0000) >> 4) | ((in & 0x3F00) >> 2) | (in & 0x00003F);

	unsigned short  SBase = 0xAC00;
	unsigned int LBase = 0xE18480;
	unsigned int VBase = 0xE185A1;
	unsigned int TBase = 0xE186A7;
	unsigned short VCount = 21;
	unsigned short TCount = 28;
	unsigned short NCount = VCount * TCount;
	unsigned short SIndex = val - SBase;
	
	*L = LBase + SIndex / NCount;
	*V = VBase + (SIndex % NCount) / TCount;
	*T = TBase + SIndex % TCount;
	if (*T == TBase) *T = 0;
}
*/

static unsigned short _getChar(unsigned int in)
{
	if (in == 0)
		return 0;

	for (unsigned short i = 0; i < TABLE_SIZE; i++)
	{
		if (characterTable[i] == in)
			return i;
	}

	return 0;
}

static unsigned short _getCharKorean(unsigned int in)
{
	if (in == 0)
		return 0;

	for (unsigned short i = 0; i < 50*50; i++)
	{
		if (koreanTable[i] == in)
			return i;
	}

	return 50*49;
}

static bool _readCharacter(FILE* f, unsigned int* val)
{
	*val = 0;
	bool newline = false;

	for (int i = 0; i < 3; i++)
	{
		unsigned char c = 0;

		if (fread(&c, 1, 1, f) == 1)
		{
			//detect new line
			if (c == 0x0A)
			{
				newline = true;
			}
			else if (c == 0x0D)
			{
				fread(&c, 1, 1, f);
				newline = true;
			}
			else
			{
				*val = (*val << 8) | c;
			}

			if (c < 128)
				break;
		}
		else
		{
			newline = true;
		}
	}

	return newline;
}

static void _readText(FILE* f, Message* m)
{	
	//clear
	for (int i = 0; i < MAX_CHARACTERS; i++)
		m->character[i] = 0;
	m->length = 0;

	unsigned int val = 0;
	bool newline = false;

	for (int pos = 0; pos < MAX_CHARACTERS; pos += 1)
	{	
		val = 0;
		newline = _readCharacter(f, &val);

		if (newline)
		{
			break;
		}
		else
		{
			//korean characters
			if (val >= 0xEAB080 && val <= 0xED9EA3)
				m->character[pos] = _getCharKorean(val) + TABLE_SIZE;
			else
				m->character[pos] = _getChar(val);
			
			m->length += 1;
		}
	}
}

static void _readBoldText(FILE* f, Message* m)
{
	char c = ' ';
	int pos = 0;

	while (1)
	{
		fread(&c, 1, 1, f);

		if (c == 0x0D)
		{
			fread(&c, 1, 1, f);
			c = '\n';
		}

		if (c == '\n' || c == EOF)
			break;

		m->character[pos] = c - ' ';
		pos++;
	}

	m->length = pos;
}

void loadText(int lang)
{
	//wrap around supported languages
	if (lang < 0) lang = LANG_END - 1;
	if (lang >= LANG_END) lang = 0;

	//
	char fpath[128];
	sprintf(fpath, "%sdata/%s.txt", SYS_DATAPATH, label[lang]);
	FILE* f = fopen(fpath, "rb");

	if (!f)
	{
		#ifdef _DEBUG
		printf("Error opening text file %s\n", fpath);
		#endif
	}
	else
	{
		//
		gameSetLanguage(lang);
		forcePlain = false;

		if (gameGetLanguage() == LANG_KOREAN)
			forcePlain = true;

		//
		for (int i = 0; i < TEXT_LAST; i++)
		{			
			//bold text
			if (i <= TEXT_THE_END && !forcePlain)
				_readBoldText(f, &(text[i]));
			//plain text
			else
				_readText(f, &(text[i]));
		}	
	}

	fclose(f);
}

int drawText(int line, float x, float y, bool center)
{
	Message* m = &text[line];

	int charW = 10;
	int charH = 16;

	if (center)
		x -= (charW-1) * m->length / 2;

	int pos = 0;
	for (int i = 0; i < m->length; i++)
	{
		if (m->character[pos] != 0)
		{
			if (m->character[pos] < TABLE_SIZE)
				gfx_DrawTile(images[imgFontKana], x, y, charW, charH, m->character[pos]);
			else
				gfx_DrawTile(images[imgFontHangul], x, y+7, 10, 10, m->character[pos] - TABLE_SIZE);
		}
		pos += 1;
		x += charW-1;		
	}

	return x;
}

int drawChar(char c, float x, float y)
{
	int charW = 10;
	int charH = 16;

	gfx_DrawTile(images[imgFontKana], x, y, charW, charH, _getChar(c));

	return x + charW - 1;
}

int messageLength(int line)
{
	Message* m = &text[line];
	return m->length;
}

bool textIsPlain()
{
	return forcePlain;
}

char* getLanguageLabel(int lang)
{
	if (lang < 0 || lang >= LANG_END)
		lang = 0;

	return label[lang];
}

int drawTextBold(int line, float x, float y, char col, bool centered)
{
	Message* m = &(text[line]);
	int pos = 0;
	int charw = 8;

	if (forcePlain)
		charw = 10;

	if (centered)
		x -= charw * (m->length / 2.f);

	for (int i = 0; i < m->length; i++)
	{
		if (m->character[pos] != 0)
		{
			if (m->character[pos] < TABLE_SIZE)
			{
				if (forcePlain)
					gfx_DrawTile(images[imgFontKana], x, y-8, 10, 16, m->character[pos]);
				else
					gfx_DrawTile(images[imgFont], x, y, 8, 8, m->character[pos] + (col * 64));
			}
			else
			{
				gfx_DrawTile(images[imgFontHangul], x, y, 10, 10, m->character[pos] - TABLE_SIZE);
			}
		}

		pos += 1;
		x += charw;
	}

	return x;
}

int drawTextBold(char const* str, float x, float y, char col, bool centered)
{
	if (forcePlain)
	{
		//Plain
		int c = 0;
		int i = 0;

		if (centered)
			x -= 9.f * (strlen(str) / 2.f);

		while (str[i] != '\0')
		{
			c = _getChar(str[i]);

			if (c != 0)
				gfx_DrawTile(images[imgFontKana], x, y-8, 10, 16, c);

			x += 9;
			i += 1;
		}

		return x;
	}

	return drawBold(str, x, y, col, centered);
}

int drawBold(char const* str, float x, float y, char col, bool centered)
{
	char c = ' ';
	int i = 0;

	if (centered)
		x -= 8.f * (strlen(str) / 2.f);

	while (str[i] != '\0')
	{
		c = str[i];

		//to uppercase
		if (c >= 'a' && c <= 'z')
			c -= 'a' - 'A';

		c -= ' ';

		if (c != 0)
			gfx_DrawTile(images[imgFont], x, y, 8, 8, c + (col * 64));

		x += 8;
		i+=1;
	}

	return x;
}