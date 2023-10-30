# include <Siv3D.hpp> // Siv3D v0.6.12
# include "lang.h"
# include "GameUIToolkit.h" 

// ウィンドウの幅
const int32 WindowSizeWidth = 1920;
// ウィンドウの高さ
const int32 WindowSizeHeight = 1017;
const int32 ObjectWidth = 4;
const int32 buf = 16;
const String PathImage = U"image0001";
const String PathMusic = U"music001";
const String PathSound = U"sound001";

enum class Language {
	English,
	Japan,
	C,
	Deutsch
};
Language language;
LangString langString;

class Lang {
public:
	String RestBasket;
	String Score;
	String StringTheRemainingTime;
	String BackGame;
	String GameEnd;
	String RouletteResult;
};
Lang lang;

enum class KindArc
{
	Weapon,
	Armor,
	Unique
};
class Arc : P2Body {
public:
	KindArc KA;
};

enum class RouletteResultMode
{
	Display,
	Delete,
	Move
};

// フェードイン・フェードアウトの描画をするクラス
struct IFade
{
	virtual ~IFade() = default;
	virtual void fade(double t) = 0;
};
struct Fade4 : public IFade
{
	Array<int> rectPos;
	const int size = 50;
	const int w = Scene::Width() / size;
	const int h = Scene::Height() / size;

	Fade4()
	{
		rectPos = Iota(w * h);
		Shuffle(rectPos);
	}

	void fade(double t) override
	{
		for (auto [index, pos] : Indexed(rectPos))
		{
			if (index > t * w * h) break;

			const auto x = pos % w;
			const auto y = pos / w;
			Rect{ x * size, y * size, size }.draw(Color{ 0,0,0,255 });
		}
	}
};
auto randomFade()
{
	Array<std::function<std::unique_ptr<IFade>()>> makeFadeFuncs = {
		[]() -> std::unique_ptr<IFade> { return std::make_unique<Fade4>(); },
	};

	return Sample(makeFadeFuncs)();
}

enum class EnumSegment
{
	Sword,
	Armor,
	Arc
};
/// @brief ホイールのセグメント
struct Segment
{
	EnumSegment es;

	// ラベル
	String text;

	// 開始角度（ラジアン）
	double startAngle;

	// 角度（ラジアン）
	double angle;

	// 色
	ColorF color;

	// フォントの大きさ
	double fontSize;

	// 起こりやすさ（他のセグメントと相対的）
	double p;

	// 境界ぎりぎりでないセグメントの領域を返す（ラジアン）
	std::pair<double, double> getArea(double epsilon) const
	{
		// 境界ぎりぎりにならないような角度の範囲を返す
		return{ (startAngle + epsilon * 10), (startAngle + angle - epsilon * 10) };
	}
};


/// @brief 共有するデータ構造体
struct GameData
{
	Audio audio;
	double audioVol = 2.5;
	Audio audioSE;
	double audioVolSE = 2.5;

	/// @brief クリアフラグ
	bool clearFlag = false;

	long G = 0;

	/// @brief 
	Font fontNormal = Font{ 50, U"font/DotGothic16-Regular.ttf" ,FontStyle::Bitmap };
	/// @brief 
	Font fontHeadline = Font{ 50, U"font/DotGothic16-Regular.ttf" ,FontStyle::Bitmap };
	/// @brief 
	Font fontWheel = Font{ 30, U"font/DotGothic16-Regular.ttf" ,FontStyle::Bitmap };
	Font fontWheelResult = Font{ 20, U"font/DotGothic16-Regular.ttf" ,FontStyle::Bitmap };

	const Slice9 slice9{ U"001_Warehouse/001_DefaultGame/001_SystemImage/wnd0.png", Slice9::Style{
						.backgroundRect = Rect{ 0, 0, 64, 64 },
						.frameRect = Rect{ 64, 0, 64, 64 },
						.cornerSize = 8,
						.backgroundRepeat = false
						} };

};

using App = SceneManager<String, GameData>;

////Base

/// @brief 言語選択シーン
class SelectLang : public App::Scene
{
public:
	// コンストラクタ（必ず実装）
	SelectLang(const InitData& init)
		: IScene{ init }
	{
		language = Language::English;
		// 画像を使用する為のTexture宣言
		String lan001 = PathImage + U"/waku0001.png";
		String lan002 = PathImage + U"/waku0002.png";
		textureWaku001 = Texture{ lan001, TextureDesc::Unmipped };
		textureWaku002 = Texture{ lan002, TextureDesc::Unmipped };
		int32 yohakuHidari = (Scene::Size().x / 2) - (wakuZentaiYoko / 2);
		// 左縦
		for (size_t i = 0; i < wakuTateLength; i++)
		{
			rectWaku001Hidari.push_back(Rect{ yohakuHidari, 60 + (i * wakuHenPX),wakuHenPX,wakuHenPX });
		}
		// 右縦
		for (size_t i = 0; i < wakuTateLength; i++)
		{
			rectWaku001Migi.push_back(Rect{ Scene::Size().x - yohakuHidari - wakuHenPX, 60 + (i * wakuHenPX),wakuHenPX,wakuHenPX });
		}
		// 上横
		for (size_t i = 0; i < wakuTateLength; i++)
		{
			rectWaku001Ue.push_back(Rect{ yohakuHidari + wakuHenPX + (i * wakuHenPX), 60 ,wakuHenPX,wakuHenPX });
		}
		// 下横
		for (size_t i = 0; i < wakuTateLength; i++)
		{
			rectWaku001Shita.push_back(Rect{ yohakuHidari + wakuHenPX + (i * wakuHenPX), 810 ,wakuHenPX,wakuHenPX });
		}
		// 左縦
		for (size_t i = 0; i < wakuTateLength002; i++)
		{
			rectWaku002Hidari.push_back(Rect{ yohakuHidari - 15, 60 + (i * wakuHenPX002),wakuHenPX002,wakuHenPX002 });
		}
		// 右縦
		for (size_t i = 0; i < wakuTateLength002; i++)
		{
			rectWaku002Migi.push_back(Rect{ Scene::Size().x - yohakuHidari - wakuHenPX002 + 15, 60 + (i * wakuHenPX002),wakuHenPX002,wakuHenPX002 });
		}
		// 上横
		for (size_t i = 0; i < wakuTateLength002; i++)
		{
			rectWaku002Ue.push_back(Rect{ yohakuHidari + (i * wakuHenPX002) + (150), 45 ,wakuHenPX002,wakuHenPX002 });
		}
		// 下横
		for (size_t i = 0; i < wakuTateLength002; i++)
		{
			rectWaku002Shita.push_back(Rect{ yohakuHidari + (i * wakuHenPX002) + (150), 960 ,wakuHenPX002,wakuHenPX002 });
		}

		text1 = U"English";
		text2 = U"日本語";
		text3 = U"Select";
		text4 = U"Language";

		text5 = U"Deutsch";

		// fontNormal を使って textPlayGame を pos の位置に描画したときのテキストの領域を取得
		RectF rectText = getData().fontNormal(text1).region();
		RectF rectText2 = getData().fontNormal(text2).region();
		RectF rectText5 = getData().fontNormal(text5).region();

		RectF rectText3 = getData().fontHeadline(text3).region();
		RectF rectTex4 = getData().fontHeadline(text4).region();

		buttonEngX = (Scene::Size().x / 2) - (rectText.w / 2);
		buttonEngY = (Scene::Size().y - 600);
		buttonJaX = (Scene::Size().x / 2) - (rectText2.w / 2);
		buttonJaY = (Scene::Size().y - 490);
		buttonDeutschX = (Scene::Size().x / 2) - (rectText5.w / 2);
		buttonDeutschY = (Scene::Size().y - 380);
		lblSelectX = (Scene::Size().x / 2) - (rectText3.w) - 30;
		lblSelectY = (Scene::Size().y - 850);
		lblLanguageX = (Scene::Size().x / 2) + 30;
		lblLanguageY = (Scene::Size().y - 800);

		int32 marumi = 10;
		rectBtnEng = { buttonEngX, buttonEngY,rectText.w,rectText.h ,marumi };
		rectBtnJa = { buttonJaX, buttonJaY,rectText2.w,rectText2.h,marumi };
		rectBtnDeutsch = { buttonDeutschX, buttonDeutschY,rectText5.w,rectText5.h,marumi };
		rectSelect = { lblSelectX, lblSelectY,rectText3.w,rectText3.h,marumi };
		rectLanguage = { lblLanguageX, lblLanguageY,rectTex4.w,rectTex4.h,marumi };
	}

	// 更新関数（オプション）
	void update() override
	{
		//押されたら次の画面へ進む
		if (rectBtnEng.leftClicked())
		{
			AudioAsset(U"click").play();
			language = Language::English;

			//LangFunc(language);

			// 遷移
			changeScene(U"PlayGame", 0.9s);
		}
		if (rectBtnJa.leftClicked())
		{
			AudioAsset(U"click").play();
			language = Language::Japan;

			LangFunc(language);

			// 遷移
			changeScene(U"PlayGame", 0.9s);
		}
		if (rectBtnDeutsch.leftClicked())
		{
			AudioAsset(U"click").play();
			language = Language::Deutsch;

			LangFunc(language);

			// 遷移
			changeScene(U"PlayGame", 0.9s);
		}
	}

	void LangFunc(Language lan)
	{
		const JSON jsonLang = JSON::Load(U"lang/langString.json");

		if (not jsonLang) // もし読み込みに失敗したら
		{
			throw Error{ U"Failed to load `langString.json`" };
		}

		for (const auto& [key, value] : jsonLang[U"lang"]) {

			if (
				(lan == Language::English && value[U"lang"].getString() == U"eng")
				||
				(lan == Language::Japan && value[U"lang"].getString() == U"japan")
				||
				(lan == Language::Deutsch && value[U"lang"].getString() == U"deu")
				)
			{
				langString.TopMenuTitle = value[U"TopMenuTitle"].getString();
				langString.TopMenuPlayGame = value[U"TopMenuPlayGame"].getString();
				langString.TopMenuSelectLang = value[U"TopMenuSelectLang"].getString();
				langString.TopMenuOption = value[U"TopMenuOption"].getString();
				langString.TopMenuExit = value[U"TopMenuExit"].getString();
				langString.TopMenuCredit = value[U"TopMenuCredit"].getString();
				langString.TopMenuDiscord = value[U"TopMenuDiscord"].getString();
				langString.TopMenuComic = value[U"TopMenuComic"].getString();

				langString.OptionMenuBGM = value[U"OptionMenuBGM"].getString();
				langString.OptionMenuSE = value[U"OptionMenuSE"].getString();
				langString.OptionMenuBackMenu = value[U"OptionMenuBackMenu"].getString();

				langString.CreditMenuText = value[U"CreditMenuText"].getString();
				langString.CreditMenuBackMenu = value[U"CreditMenuBackMenu"].getString();
			}
		}
	}

	// 描画関数（オプション） 
	void draw() const override
	{
		//// 背景を描画
		// 左縦
		for (size_t i = 0; i < wakuTateLength; i++)
		{
			rectWaku001Hidari[i](textureWaku001).draw();
		}
		// 右縦
		for (size_t i = 0; i < wakuTateLength; i++)
		{
			rectWaku001Migi[i](textureWaku001).draw();
		}
		// 上横
		for (size_t i = 0; i < wakuTateLength; i++)
		{
			rectWaku001Ue[i](textureWaku001).draw();
		}
		// 下横
		for (size_t i = 0; i < wakuTateLength; i++)
		{
			rectWaku001Shita[i](textureWaku001).draw();
		}
		// 左縦
		for (size_t i = 0; i < wakuTateLength002; i++)
		{
			rectWaku002Hidari[i](textureWaku002).draw();
		}
		// 右縦
		for (size_t i = 0; i < wakuTateLength002; i++)
		{
			rectWaku002Migi[i](textureWaku002).draw();
		}
		// 上横
		for (size_t i = 0; i < wakuTateLength002; i++)
		{
			rectWaku002Ue[i](textureWaku002).draw();
		}
		// 下横
		for (size_t i = 0; i < wakuTateLength002; i++)
		{
			rectWaku002Shita[i](textureWaku002).draw();
		}

		////ボタン描写
		rectBtnEng.draw(Arg::top = ColorF{ 0.5 }, Arg::bottom = ColorF{ 1.0 }).drawFrame(0, 6, Palette::Orange);
		rectBtnJa.draw(Arg::top = ColorF{ 0.5 }, Arg::bottom = ColorF{ 1.0 }).drawFrame(0, 6, Palette::Orange);
		rectBtnDeutsch.draw(Arg::top = ColorF{ 0.5 }, Arg::bottom = ColorF{ 1.0 }).drawFrame(0, 6, Palette::Orange);
		getData().fontNormal(text1).draw(buttonEngX, buttonEngY, ColorF{ 0.25 });
		getData().fontNormal(text2).draw(buttonJaX, buttonJaY, ColorF{ 0.25 });
		getData().fontNormal(text5).draw(buttonDeutschX, buttonDeutschY, ColorF{ 0.25 });
		getData().fontHeadline(text3).draw(lblSelectX, lblSelectY, Palette::White);
		getData().fontHeadline(text4).draw(lblLanguageX, lblLanguageY, Palette::White);
	}

	void drawFadeIn(double t) const override
	{
		draw();

		m_fadeInFunction->fade(1 - t);
	}
	void drawFadeOut(double t) const override
	{
		draw();

		m_fadeOutFunction->fade(t);
	}
private:
	////枠情報
	const int32 wakuTateLength = 6;
	const int32 wakuHenPX = 150;
	const int32 wakuTateLength002 = 60;
	const int32 wakuHenPX002 = 15;
	const int32 wakuZentaiYoko = wakuHenPX * wakuTateLength + (wakuHenPX * 2);
	const int32 wakuZentaiTate = wakuHenPX * wakuTateLength;
	Texture textureWaku001;
	Texture textureWaku002;
	////枠配列
	//左縦
	Array<Rect> rectWaku001Hidari;
	//右縦
	Array<Rect> rectWaku001Migi;
	//上横
	Array<Rect> rectWaku001Ue;
	//下横
	Array<Rect> rectWaku001Shita;
	//左縦
	Array<Rect> rectWaku002Hidari;
	//右縦
	Array<Rect> rectWaku002Migi;
	//上横
	Array<Rect> rectWaku002Ue;
	//下横
	Array<Rect> rectWaku002Shita;
	////ボタン情報
	String text1;
	int32 buttonEngX = -1;
	int32 buttonEngY = -1;
	String text2;
	int32 buttonJaX = -1;
	int32 buttonJaY = -1;
	String text3;
	int32 lblSelectX = -1;
	int32 lblSelectY = -1;
	String text4;
	int32 lblLanguageX = -1;
	int32 lblLanguageY = -1;
	String text5;
	int32 buttonDeutschX = -1;
	int32 buttonDeutschY = -1;
	RoundRect rectBtnEng;
	RoundRect rectBtnDeutsch;
	RoundRect rectBtnJa;
	RoundRect rectSelect;
	RoundRect rectLanguage;

	std::unique_ptr<IFade> m_fadeInFunction = randomFade();
	std::unique_ptr<IFade> m_fadeOutFunction = randomFade();
};
class PlayGame : public App::Scene
{
public:
	PlayGame(const InitData& init)
		: IScene{ init }
	{
		Profiler::EnableAssetCreationWarning(false);

		lang.RestBasket = U"籠を定位置へ戻す";
		lang.Score = U"売却金: {} ゴレドー";
		lang.StringTheRemainingTime = U"残り時間";
		lang.GameEnd = U"お疲れ様！";
		lang.RouletteResult = U"ただいま、{}が高額買取中です！！";

		RectF re = getData().fontHeadline(lang.RestBasket).region();
		rectFScore = Rect{ WindowSizeWidth - 600,WindowSizeHeight - 300 + 50,600,(int32)re.h };
		rectFScenario1X = Rect{ WindowSizeWidth - 600,WindowSizeHeight - 300 + 50 + (int32)re.h + 30,(int32)re.w,(int32)re.h };

		rectFBackTop = Rect{ 0,0 ,WindowSizeWidth,30 };
		rectFBackBottom = Rect{ 0,WindowSizeHeight - 30,WindowSizeWidth ,30 };

		idPBasket001Collsion = pBasket001Collsion.id();
		idPBasket002Collsion = pBasket002Collsion.id();
		idPBasket003Collsion = pBasket003Collsion.id();
		idWheel = wheel.id();
		idPoi = poi.id();

		stopwatch.start();
		stopwatchWheelStop.set(10s);

		AudioAsset(U"Funk__BPM126").play();
	}

	// 更新関数（オプション）
	void update() override
	{
		if (Speak && (stopwatchGE.s() > 0.5))
		{
			if (TextToSpeech::IsSpeaking() == false)
			{
				getData().G = point;
				// 遷移
				changeScene(U"Consequence", 0.9s);
			}
		}

		if (TheRemainingTime - stopwatch.s() < 0 && Speak == false)
		{
			AudioAsset(U"click").play();
			TextToSpeech::Speak(U"Let's conseaquence, shall we?");
			stopwatchGE.start();
			Speak = true;
		}

		for (accumulatorSec += Scene::DeltaTime(); stepSec <= accumulatorSec; accumulatorSec -= stepSec)
		{
			world.update(stepSec);
		}

		Circle c = wheel.as<P2Circle>(0)->getCircle();
		Polygon p = poi.as<P2Polygon>(0)->getPolygon();

		// ボールの追加
		if ((c.x < WindowSizeWidth - 950 && c.x > 0)
			&& (c.y > WindowSizeHeight - 350))
		{
			if (p.centroid().y > WindowSizeHeight - 350)
			{
				//摩擦が低いほど回転しない
				std::pair<P2Body, KindArc> pp(world.createCircle(P2Dynamic, p.centroid(), Random(10.0, 15.0), P2Material{ .density = 0.001, .restitution = 0.5, .friction = 1.0 }), KindArc::Weapon);
				int32 r = Random(0, 2);
				if (r == 0)
				{
					pp.second = KindArc::Weapon;
				}
				else if (r == 1)
				{
					pp.second = KindArc::Armor;
				}
				else if (r == 2)
				{
					pp.second = KindArc::Unique;
				}
				balls.emplace_back(pp);
			}
		}

		//アクション
		{
			if (c.leftClicked())
			{
				if (MouseL.down())
				{
					mouseJoint = world.createMouseJoint(wheel, Cursor::PosF())
						.setMaxForce(wheel.getMass() * 5000.0)
						.setLinearStiffness(2.0, 0.7);
				}
				else if (MouseL.up())
				{
					mouseJoint.release();
				}
			}
			else
			{
				if (MouseL.pressed())
				{
					mouseJoint.setTargetPos(Cursor::PosF());
				}
				else if (MouseL.up())
				{
					mouseJoint.release();
				}
			}

			if (rectFScenario1X.leftClicked())
			{
				wheel.release();
				wheel = GetWheel(world);

				poi.release();
				poi = GetPoi(world);

				wheelJoint.release();
				wheelJoint = CreateWheelJoint(world);

				idWheel = wheel.id();
				idPoi = poi.id();

				//KlattTTS::Speak(U"Release and get!");
				Say(U"Release and get!");
			}
		}

		{
			auto&& getC = world.getCollisions();

			// こぼれたボールの削除
			balls.remove_if([&](const std::pair<P2Body, KindArc>& b)
				{
					if ((Scene::Height() + 1000 < b.first.getPos().y || Scene::Width() < b.first.getPos().x) == true)
					{
						return true;
					}
					return false;
				});
			// 籠のボールの削除
			Array<int32> idCi001;
			Array<int32> idCi002;
			Array<int32> idCi003;
			for (auto&& [pair, collision] : getC)
			{
				if (pair.a == idPBasket001Collsion)
				{
					if (pair.b != idWheel && pair.b != idPoi)
					{
						idCi001.push_back(pair.b);
						continue;
					}
				}
				if (pair.a == idPBasket002Collsion)
				{
					if (pair.b != idWheel && pair.b != idPoi)
					{
						idCi002.push_back(pair.b);
						continue;
					}
				}
				if (pair.a == idPBasket003Collsion)
				{
					if (pair.b != idWheel && pair.b != idPoi)
					{
						idCi003.push_back(pair.b);
						continue;
					}
				}
			}
			balls.remove_if([&](const std::pair<P2Body, KindArc>& b)
				{
					if (idCi001.contains(b.first.id()))
					{
						switch (b.second)
						{
						case KindArc::Weapon:
						{
							if (segments[selectedSeg].es == EnumSegment::Sword)
							{
								point = point + 15;
							}
							else
							{
								point = point + 5;
							}
						}
						break;
						case KindArc::Armor:
						{
							if (segments[selectedSeg].es == EnumSegment::Sword)
							{
								point = point + 3;
							}
							else
							{
								point = point + 1;
							}
						}
						break;
						case KindArc::Unique:
						{
							if (segments[selectedSeg].es == EnumSegment::Sword)
							{
								point = point + 3;
							}
							else
							{
								point = point + 1;
							}
						}
						break;
						default:
							break;
						}
						AudioAsset(U"gun").play();
						return true;
					}
					if (idCi002.contains(b.first.id()))
					{
						switch (b.second)
						{
						case KindArc::Weapon:
						{
							if (segments[selectedSeg].es == EnumSegment::Armor)
							{
								point = point + 3;
							}
							else
							{
								point = point + 1;
							}
						}
						break;
						case KindArc::Armor:
						{
							if (segments[selectedSeg].es == EnumSegment::Armor)
							{
								point = point + 15;
							}
							else
							{
								point = point + 5;
							}
						}
						break;
						case KindArc::Unique:
						{
							if (segments[selectedSeg].es == EnumSegment::Armor)
							{
								point = point + 3;
							}
							else
							{
								point = point + 1;
							}
						}
						break;
						default:
							break;
						}
						AudioAsset(U"gun").play();
						return true;
					}
					if (idCi003.contains(b.first.id()))
					{
						switch (b.second)
						{
						case KindArc::Weapon:
						{
							if (segments[selectedSeg].es == EnumSegment::Arc)
							{
								point = point + 3;
							}
							else
							{
								point = point + 1;
							}
						}
						break;
						case KindArc::Armor:
						{
							if (segments[selectedSeg].es == EnumSegment::Arc)
							{
								point = point + 3;
							}
							else
							{
								point = point + 1;
							}
						}
						break;
						case KindArc::Unique:
						{
							if (segments[selectedSeg].es == EnumSegment::Arc)
							{
								point = point + 15;
							}
							else
							{
								point = point + 5;
							}
						}
						break;
						default:
							break;
						}
						AudioAsset(U"gun").play();
						return true;
					}

					return false;
				});
		}

		//ルーレット
		{
			if (not targetAngle) // まだ最終的な回転角度が決まっていない場合
			{
				// 回転を開始してからの経過時間に基づいて角速度を増やす
				angularVelocity = Min((stopwatchWheel.sF() * WarmupAngularAcceleration), MaxAngularVelocity);

				// 現在の角速度に基づいて回転角度を増やす
				angle += (angularVelocity * Scene::DeltaTime());
			}
			else // 最終的な回転角度が決まっている（Stop を押した）場合
			{
				// 最終的な回転角度に向けてスムーズに移動する（角速度も変化）
				angle = Math::SmoothDamp(angle, *targetAngle, angularVelocity, 0.5, MaxAngularVelocity);

				// 最終的な回転角度に近づいたら
				if (AbsDiff(angle, *targetAngle) < Epsilon)
				{
					// 現在の回転角度を最終的な回転角度にする
					angle = *targetAngle;

					// 角速度を 0 にする
					angularVelocity = 0_deg;

					// 最終的な回転角度をリセットする
					targetAngle.reset();

					// ストップウォッチをリセットする
					stopwatchWheel.reset();

					stopwatchWheelStop.restart();

					for (size_t i = 0; i < 1; i++)
					{
						arrayRouletteResult.push_back(RectF{ -120,60 + (i * 100),600,100 });
					}
				}
			}

			if (stopwatchWheel < WarmupTime) // ウォームアップ時間が経過していない場合
			{
				// 「Start」ボタンを表示する
				// stopwatch が開始していない時だけ押せる
				if (not stopwatchWheel.isRunning() && (stopwatchWheelStop >= StopTime))
				{
					if (arrayRouletteResult.size() > 0)
					{
						rrm = RouletteResultMode::Move;
					}

					// 回転を開始してからの経過時間を測定開始する
					stopwatchWheel.restart();
				}
			}
			else
			{
				// 「Stop」ボタンを表示する
				if (not targetAngle)
				{
					// 確率に基づき抽選結果を決定する
					const size_t selected = distribution(GetDefaultRNG());
					selectedSeg = selected;
					// 抽選結果に合うセグメントの角度範囲を取得する
					const auto [min, max] = segments[selected].getArea(Epsilon);

					// 抽選結果に合う角度をランダムに決定する
					const double result = Random(min, max);

					// 抽選結果に合う最終的な回転角度を計算する
					// (現在の回転角度) + (360° までの角度) + (抽選結果を指すための回転角度) + (余分な周回 (360° の倍数))
					targetAngle = angle + (360_deg - Fmod(angle, 360_deg)) + (360_deg - result) + (360_deg * 1);

					// デバッグ表示
					//ClearPrint();
					//Print << segments[selected].text;
					//Print << ToDegrees(result);
				}
			}
		}

		//ルーレット結果エフェクト
		{
			switch (rrm)
			{
			case RouletteResultMode::Display:
				for (auto& aaa : arrayRouletteResult)
				{
					if (aaa.x < 12)
					{
						// 移動
						aaa.x = aaa.x + 12;
					}
				}
				break;
			case RouletteResultMode::Delete:
			{
				arrayRouletteResult.remove_if([&](const RectF& rf)
					{
						if (rf.y + heightArrayRouletteResult <= 0)
						{
							rrm = RouletteResultMode::Display;
							return true;
						}
						return false;
					});
			}
			break;
			case RouletteResultMode::Move:
			{
				for (auto& aaa : arrayRouletteResult)
				{
					// 移動
					aaa.y = aaa.y - 12;

					if (aaa.y + heightArrayRouletteResult <= 0)
					{
						rrm = RouletteResultMode::Delete;
					}
				}
			}
			break;
			default:
				break;
			}
		}
	}

	void draw() const override
	{
		TextureAsset(U"background").draw();
		TextureAsset(U"land").draw(WindowSizeWidth - 950 - ObjectWidth, WindowSizeHeight - 300 - ObjectWidth);
		//pLand001.draw(ColorF{ 0.3, 0.8, 0.5 });
		//pLand002.draw(ColorF{ 0.3, 0.8, 0.5 });
		pBasket001.draw(ColorF(U"#003366"));
		pBasket002.draw(ColorF(U"#006699"));
		pBasket003.draw(ColorF(U"#ff9900"));
		wheel.draw(ColorF{ 0.3, 0.8, 0.5 });
		poi.draw(ColorF{ 0.3, 0.8, 0.5 });
		getData().slice9.draw(rectFScenario1X.asRect());
		getData().fontHeadline(lang.RestBasket).draw(rectFScenario1X, ColorF{ 1.0 });
		getData().slice9.draw(rectFScore.asRect());
		getData().fontHeadline(lang.Score.replaced(U"{}", Format(point))).draw(rectFScore, ColorF{ 1.0 });
		if (TheRemainingTime - stopwatch.s() < 0)
		{
			getData().fontHeadline(lang.GameEnd)
				.draw(WindowSizeWidth - 600 - 16 - 350, 50, ColorF{ 1.0 });
		}
		else
		{
			getData().fontHeadline(lang.StringTheRemainingTime + Format(TheRemainingTime - stopwatch.s()))
				.draw(WindowSizeWidth - 600 - 16 - 350, 50, ColorF{ 1.0 });
		}

		for (const auto& ball : balls)
		{
			switch (ball.second)
			{
			case KindArc::Weapon:
			{
				TextureAsset(U"sword").resized(ball.first.as<P2Circle>(0)->getCircle().r * 2).rotated(ball.first.getAngle()).drawAt(ball.first.as<P2Circle>(0)->getCircle().center);
			}
			break;
			case KindArc::Armor:
			{
				TextureAsset(U"armor").resized(ball.first.as<P2Circle>(0)->getCircle().r * 2).rotated(ball.first.getAngle()).drawAt(ball.first.as<P2Circle>(0)->getCircle().center);
			}
			break;
			case KindArc::Unique:
			{
				TextureAsset(U"arc001").resized(ball.first.as<P2Circle>(0)->getCircle().r * 2).rotated(ball.first.getAngle()).drawAt(ball.first.as<P2Circle>(0)->getCircle().center);
			}
			break;
			default:
				break;
			}
		}

		TextureAsset(U"sea")
			.resized(WindowSizeWidth - 950 - ObjectWidth, 300 + buf)
			.draw(0, WindowSizeHeight - 300 - ObjectWidth);

		rectFBackTop.draw(ColorF(U"#111111"));
		rectFBackBottom.draw(ColorF(U"#111111"));

		// ホイールを描く
		DrawWheel(angle, segments, getData().fontWheel);

		for (auto& aaa : arrayRouletteResult)
		{
			getData().slice9.draw(aaa.asRect());
			getData().fontWheelResult(lang.RouletteResult.replaced(U"{}", segments[selectedSeg].text)).draw(aaa, ColorF{ 1.0 });
		}

		if (stopwatch.s() < 10)
		{
			TextureAsset(U"sword")
				.draw(WindowSizeWidth - 500, WindowSizeHeight - 700 - 50);
			TextureAsset(U"armor")
				.draw(WindowSizeWidth - 350, WindowSizeHeight - 700 - 50);
			TextureAsset(U"arc001")
				.draw(WindowSizeWidth - 200, WindowSizeHeight - 700 - 50);
			TextureAsset(U"arc002")
				.draw(WindowSizeWidth - 230, WindowSizeHeight - 700 - 0);
			TextureAsset(U"arc003")
				.draw(WindowSizeWidth - 170, WindowSizeHeight - 700 - 0);
			lBasket001.drawArrow(3, Vec2{ 20, 20 }, Palette::Skyblue);
			lBasket002.drawArrow(3, Vec2{ 20, 20 }, Palette::Skyblue);
			lBasket003.drawArrow(3, Vec2{ 20, 20 }, Palette::Skyblue);
			getData().fontNormal(U"籠を海の中に入れて、\r\nアイテムをゲットし、\r\n左の籠の中に入れて換金しよう！").draw(60, 80, ColorF{ 0 });
		}
	}

	void drawFadeIn(double t) const override
	{
		draw();

		m_fadeInFunction->fade(1 - t);
	}
	void drawFadeOut(double t) const override
	{
		draw();

		m_fadeOutFunction->fade(t);
	}
	P2Body GetPoi(P2World world)
	{
		return world.createPolygon(P2Dynamic, Vec2{ 1100, 350 },
										LineString{ Vec2{-100, 50},
													Vec2{-100, 150},
													Vec2{100, 150},
													Vec2{100, 50} }
		.calculateBuffer(5), P2Material{ .friction = 1.0 });
	}
	P2Body GetWheel(P2World world)
	{
		return world.createCircle(P2Dynamic, Vec2{ 1100, 300 }, 20);
	}
	P2WheelJoint CreateWheelJoint(P2World world)
	{
		return world.createWheelJoint(poi, wheel, wheel.getPos(), Vec2{ 0, 1 })
			.setLimitsEnabled(true);
	}
	int32 GetPlusScore(int32 arg)
	{
		switch (segments[selectedSeg].es)
		{
		case EnumSegment::Sword:
			break;
		case EnumSegment::Armor:
			break;
		case EnumSegment::Arc:
			break;
		default:
			break;
		}

		return 0;
	}
	/// @brief ホイールを描画する
	/// @param angle 
	/// @param segments 
	/// @param font 
	void DrawWheel(double angle, const Array<Segment>& segments, const Font& font) const
	{
		// ホイールを表現する円
		constexpr Circle Wheel{ WindowSizeWidth - 950 + 200, WindowSizeHeight - 300 + 150, 125 };

		// 各セグメントを描く
		for (const auto& segment : segments)
		{
			// セグメントを描く
			Wheel.drawPie(angle + segment.startAngle, segment.angle, segment.color);

			{
				// 回転角度に応じてテキスト描画を回転させる
				const Transformer2D tr{ Mat3x2::Rotate(segment.startAngle + segment.angle * 0.5 + angle - 90_deg, Wheel.center) };

				// 輪郭付きでテキストを描く
				font(segment.text).draw(TextStyle::Outline(0.25, ColorF{ 0.1 }), segment.fontSize, Arg::rightCenter = Wheel.center.movedBy(Wheel.r - 10, 0));
			}
		}

		// ホイールの枠を描く
		Wheel.drawFrame(1, 4);

		// ホイールの中心を描く
		Circle{ Wheel.center, 40 }
			.draw(Scene::GetBackground())
			.drawFrame(5); // 枠を描く

		// 矢印の三角
		constexpr Triangle triangle{ { (Wheel.center.x - 15), (Wheel.center.y - Wheel.r - 30)},
			{ (Wheel.center.x + 15), (Wheel.center.y - Wheel.r - 30)}, { (Wheel.center.x), (Wheel.center.y - Wheel.r + 15)} };

		// 矢印のキャップ
		constexpr Circle circle = Circle{ ((triangle.p0 + triangle.p1) / 2).movedBy(0, -3), 15.5 };

		// 矢印を描く
		triangle.stretched(2).draw(ColorF{ 0.3 });
		circle.stretched(2).draw(ColorF{ 0.3 });
		triangle.draw(ColorF{ 0.8 });
		circle.draw(ColorF{ 0.8 });
	}
	/// @brief 各セグメントの選ばれやすさに基づいて DiscreteDistribution を作成する
	/// @param segments 
	/// @return 
	DiscreteDistribution MakeDiscreteDistribution(const Array<Segment>& segments)
	{
		Array<double> ps;

		for (const auto& segment : segments)
		{
			ps << segment.p;
		}

		return DiscreteDistribution{ ps };
	}

	// セグメント一覧
	// セグメントの面積と選ばれる確率は独立して設定可能
	const Array<Segment> segments =
	{
		// 「$10」というセグメント, 0° から始まり, 60° の角度, 黄色, フォントサイズ 64, 選ばれやすさ 1000
		{EnumSegment::Sword,U"Sword", 0_deg, 60_deg, ColorF(U"#111111"), 32, 1000},

		// 「$20」というセグメント, 60° から始まり, 30° の角度, 緑色, フォントサイズ 48, 選ばれやすさ 500
		{EnumSegment::Armor,U"Armor", 60_deg, 30_deg, HSV{90, 0.9, 0.9}, 16, 500},

		// 以下同様に設定
		{EnumSegment::Sword,U"Sword", 90_deg, 30_deg, ColorF(U"#111111"), 32, 700},
		{EnumSegment::Arc,U"Arc", 120_deg, 60_deg, HSV{150, 0.9, 0.9}, 16, 300},
		{EnumSegment::Sword,U"Sword", 180_deg, 60_deg, ColorF(U"#111111"), 32, 1000},
		{EnumSegment::Arc,U"Arc", 240_deg, 30_deg, HSV{210, 0.9, 0.9}, 16, 300},
		{EnumSegment::Sword,U"Sword", 270_deg, 60_deg, ColorF(U"#111111"), 32, 1000},
		{EnumSegment::Armor,U"Armor", 330_deg, 30_deg, HSV{150, 0.9, 0.9}, 16, 500},
	};
	size_t selectedSeg = -1;
	// 出現確率分布を計算するクラス
	DiscreteDistribution distribution = MakeDiscreteDistribution(segments);

	// ウォームアップにかかる時間
	Duration WarmupTime = 1.5s;

	// 
	Duration StopTime = 10s;

	// ウォームアップ中の角加速度（ラジアン/秒）
	double WarmupAngularAcceleration = 360_deg;

	// 最大の角速度（ラジアン/秒）
	double MaxAngularVelocity = (WarmupTime.count() * WarmupAngularAcceleration);

	// 回転を停止する角度
	double Epsilon = 0.1_deg;

	// 角速度（ラジアン/秒）
	double angularVelocity = 0_deg;

	// 現在の回転角度（ラジアン）
	double angle = -255_deg;

	// 最終的な回転角度（ラジアン）
	Optional<double> targetAngle;

	// 回転を開始してからの経過時間
	Stopwatch stopwatchWheel;
	Stopwatch stopwatchWheelStart;
	Stopwatch stopwatchWheelStop;


	//UI
	RectF rectFScenario1X; RectF rectFScore; RectF rectFBackTop; RectF rectFBackBottom;
	Stopwatch stopwatchGE; Stopwatch stopwatch;
	int32 TheRemainingTime = 50;
	Array<RectF> arrayRouletteResult;
	RouletteResultMode rrm = RouletteResultMode::Display;
	int32 heightArrayRouletteResult = 120;

	bool Speak = false;

	long point = 0;
	int32 idPBasket001Collsion = -1; int32 idPBasket002Collsion = -1; int32 idPBasket003Collsion = -1;
	int32 idWheel = -1; int32 idPoi = -1;
	// 2D 物理演算のシミュレーションステップ（秒）
	const double stepSec = (1.0 / 200.0);
	// 2D 物理演算のシミュレーション蓄積時間（秒）
	double accumulatorSec = 0.0;

	// 2D 物理演算のワールド
	P2World world;
	P2Body wheel = GetWheel(world);
	P2Body poi = GetPoi(world);

	// ホイールジョイント
	P2WheelJoint wheelJoint = CreateWheelJoint(world);
	// マウスジョイント
	P2MouseJoint mouseJoint;

	//ポイ置き場
	P2Body pLand001 = world.createPolygon(P2Static, Vec2{ 0, 0 },
										LineString{ Vec2{WindowSizeWidth - 950, WindowSizeHeight + 1000},
													Vec2{WindowSizeWidth - 950, WindowSizeHeight - 300},
													Vec2{WindowSizeWidth - 000, WindowSizeHeight - 300},
													Vec2{WindowSizeWidth - 000, WindowSizeHeight - 000},
													Vec2{WindowSizeWidth - 950, WindowSizeHeight - 000} }
	.calculateBuffer(5), P2Material{ .friction = 0.0 });
	//P2Body pLand002 = world.createPolygon(P2Static, Vec2{ 0, 0 },
	//									LineString{ Vec2{WindowSizeWidth - 950, WindowSizeHeight - 000},
	//												Vec2{WindowSizeWidth - 950, WindowSizeHeight - 350},
	//												Vec2{WindowSizeWidth - 600, WindowSizeHeight - 350},
	//												Vec2{WindowSizeWidth - 600, WindowSizeHeight - 000} }
	//.calculateBuffer(ObjectWidth), P2Material{ .friction = 1.0 });
	//籠
	P2Body pBasket001 = world.createPolygon(P2Static, Vec2{ 0, 0 },
										LineString{ Vec2{WindowSizeWidth - 500, WindowSizeHeight - 500},
													Vec2{WindowSizeWidth - 500, WindowSizeHeight - 400},
													Vec2{WindowSizeWidth - 400, WindowSizeHeight - 400},
													Vec2{WindowSizeWidth - 400, WindowSizeHeight - 500} }
	.calculateBuffer(5), P2Material{ .friction = 0.0 });
	Line lBasket001{ WindowSizeWidth - 500,WindowSizeHeight - 700 , WindowSizeWidth - 500, WindowSizeHeight - 500 };
	P2Body pBasket001Collsion = world.createPolygon(P2Static, Vec2{ 0, 0 },
										LineString{ Vec2{WindowSizeWidth - 500 + ObjectWidth / 2, WindowSizeHeight - 500},
													Vec2{WindowSizeWidth - 500 + ObjectWidth / 2, WindowSizeHeight - 400},
													Vec2{WindowSizeWidth - 400 - ObjectWidth / 2, WindowSizeHeight - 400},
													Vec2{WindowSizeWidth - 400 - ObjectWidth / 2, WindowSizeHeight - 500} }
	.calculateBuffer(5), P2Material{ .friction = 0.0 });

	P2Body pBasket002 = world.createPolygon(P2Static, Vec2{ 0, 0 },
										LineString{ Vec2{WindowSizeWidth - 350, WindowSizeHeight - 500},
													Vec2{WindowSizeWidth - 350, WindowSizeHeight - 400},
													Vec2{WindowSizeWidth - 250, WindowSizeHeight - 400},
													Vec2{WindowSizeWidth - 250, WindowSizeHeight - 500} }
	.calculateBuffer(5), P2Material{ .friction = 0.0 });
	Line lBasket002{ WindowSizeWidth - 350,WindowSizeHeight - 700 , WindowSizeWidth - 350, WindowSizeHeight - 500 };
	P2Body pBasket002Collsion = world.createPolygon(P2Static, Vec2{ 0, 0 },
										LineString{ Vec2{WindowSizeWidth - 350 + ObjectWidth / 2, WindowSizeHeight - 500},
													Vec2{WindowSizeWidth - 350 + ObjectWidth / 2, WindowSizeHeight - 400},
													Vec2{WindowSizeWidth - 250 - ObjectWidth / 2, WindowSizeHeight - 400},
													Vec2{WindowSizeWidth - 250 - ObjectWidth / 2, WindowSizeHeight - 500} }
	.calculateBuffer(5), P2Material{ .friction = 0.0 });

	P2Body pBasket003 = world.createPolygon(P2Static, Vec2{ 0, 0 },
										LineString{ Vec2{WindowSizeWidth - 200, WindowSizeHeight - 500},
													Vec2{WindowSizeWidth - 200, WindowSizeHeight - 400},
													Vec2{WindowSizeWidth - 100, WindowSizeHeight - 400},
													Vec2{WindowSizeWidth - 100, WindowSizeHeight - 500} }
	.calculateBuffer(5), P2Material{ .friction = 0.0 });
	Line lBasket003{ WindowSizeWidth - 200,WindowSizeHeight - 700 , WindowSizeWidth - 200, WindowSizeHeight - 500 };
	P2Body pBasket003Collsion = world.createPolygon(P2Static, Vec2{ 0, 0 },
										LineString{ Vec2{WindowSizeWidth - 200 + ObjectWidth / 2, WindowSizeHeight - 500},
													Vec2{WindowSizeWidth - 200 + ObjectWidth / 2, WindowSizeHeight - 400},
													Vec2{WindowSizeWidth - 100 - ObjectWidth / 2, WindowSizeHeight - 400},
													Vec2{WindowSizeWidth - 100 - ObjectWidth / 2, WindowSizeHeight - 500} }
	.calculateBuffer(5), P2Material{ .friction = 0.0 });

	Array<std::pair<P2Body, KindArc>> balls;

	std::unique_ptr<IFade> m_fadeInFunction = randomFade();
	std::unique_ptr<IFade> m_fadeOutFunction = randomFade();
};
class Consequence : public App::Scene
{
public:
	// コンストラクタ（必ず実装）
	Consequence(const InitData& init)
		: IScene{ init }
	{
		lang.BackGame = U"もう一度「金魚すくい」を実行する";
		RectF re = getData().fontHeadline(lang.BackGame).region();
		rectFBackGame = Rect{ (WindowSizeWidth / 2) - ((int32)re.w / 2),(WindowSizeHeight / 2) - ((int32)re.h / 2),(int32)re.w,(int32)re.h };
	}
	// 更新関数（オプション）
	void update() override
	{
		if (Speak && (stopwatchGE.s() > 0.5))
		{
			if (TextToSpeech::IsSpeaking() == false)
			{
				// 遷移
				changeScene(U"PlayGame", 0.9s);
			}
		}

		if (rectFBackGame.leftClicked())
		{
			AudioAsset(U"click").play();
			TextToSpeech::Speak(U"Let us execute, will you?");
			stopwatchGE.start();
			Speak = true;
		}

		if (SimpleGUI::Button(U"スコアをツイート", Vec2{ 40, 40 }))
		{
			// ハッシュタグや URL を含めると広まりやすいです。
			const String text = U"金魚すくいの結果 {} 点！\n#Goldfish_Scooping #Siv3D"_fmt(ThousandsSeparate(getData().G));

			// ツイート投稿画面を開く
			Twitter::OpenTweetWindow(text);
		}
	}
	// 描画関数（オプション）
	void draw() const override
	{
		getData().slice9.draw(rectFBackGame.asRect());
		getData().fontHeadline(lang.BackGame).draw(rectFBackGame, ColorF{ 1.0 });
	}

	void drawFadeIn(double t) const override
	{
		draw();

		m_fadeInFunction->fade(1 - t);
	}

	void drawFadeOut(double t) const override
	{
		draw();

		m_fadeOutFunction->fade(t);
	}
private:
	RectF rectFBackGame;
	bool Speak = false;
	Stopwatch stopwatchGE;

	std::unique_ptr<IFade> m_fadeInFunction = randomFade();
	std::unique_ptr<IFade> m_fadeOutFunction = randomFade();
};

void Main()
{
	constexpr Size SceneSize{ WindowSizeWidth, WindowSizeHeight };
	Scene::Resize(SceneSize);
	Window::SetStyle(WindowStyle::Sizable);
	Scene::SetResizeMode(ResizeMode::Keep);
	Window::Maximize();

	// 背景色を設定
	Scene::SetBackground(ColorF{ 0.2 });

	// シーンマネージャーを作成
	// ここで GameData が初期化される
	App manager;

	// シーンを登録
	//manager.add<SelectLang>(U"SelectLang");
	manager.add<PlayGame>(U"PlayGame");
	manager.add<Consequence>(U"Consequence");

	TextureAsset::Register(U"background", U"back.png");
	TextureAsset::Register(U"land", U"land.png");
	TextureAsset::Register(U"sea", U"sea.png");
	TextureAsset::Register(U"wood", U"wood.png");
	TextureAsset::Register(U"sword", U"⚔️"_emoji);
	TextureAsset::Register(U"armor", U"🛡️"_emoji);
	TextureAsset::Register(U"arc001", U"📿"_emoji);
	TextureAsset::Register(U"arc002", U"🎷"_emoji);
	TextureAsset::Register(U"arc003", U"📀"_emoji);

	// 音声ファイルへのパスを読み込み
	AudioAsset::Register(U"Funk__BPM126", PathMusic + U"/Funk__BPM126.wav");
	//効果音
	AudioAsset::Register(U"gun", PathSound + U"/tm2_gun002.wav");

	while (System::Update())
	{
		if (not manager.update())
		{
			break;
		}
	}
}
