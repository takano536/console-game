#include "title.h"
#include "game.h"

int main()
{
	bool is_continue = true;  // 実行ファイルを終了せずに続けるかどうか
	int title_choices = 0;    // 0:ゲームスタート 1:オプション 2:終了
	int game_result = 0;      // -1:異常終了 0:ゲームが始まっていない 1:ゲームオーバー 2:ゲームクリア
	bool is_retry = false;    // ゲームをリトライするかどうか
	while (is_continue)
	{
		// retry が選択されていれば title_loop に入らずゲーム開始
		if (is_retry)
			title_choices = 0;
		else
			title_choices = title_loop();  // タイトルの表示

		switch (title_choices)
		{
			case 0:  // ゲーム開始
				game_result = game_loop();
				break;

			case 1:  // オプション
				option_loop();
				break;

			case 2:  // 終了
				is_continue = false;
				break;

			default:
				break;
		}

		if (is_continue)
		{
			switch (game_result)
			{
				case -1:  // マップが読み込めなかった
					is_retry = false;
					break;

				case 0:  //オプションが呼ばれた = ゲームが始まっていない
					is_retry = false;
					break;

				case 1:  // ゲームオーバー
					is_retry = (game_end_loop("gameover") == 0 ? true : false);
					break;

				case 2:  // ゲームクリア
					is_retry = (game_end_loop("clear!!") == 0 ? true : false);
					break;

				default:
					break;
			}
		}
		game_result = 0;  // game_result を初期化
	}
	return 0;
}