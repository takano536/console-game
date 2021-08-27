#include "title.h"
#include "game.h"

int main()
{
	bool is_continue = true;
	int title_choices = 0;  // 0:ゲームスタート 1:オプション 2:終了
	int game_result = 0;    // -1:異常終了 0:ゲームが始まっていない 1:ゲームオーバー 2:ゲームクリア
	while (is_continue)
	{
		if (game_result == 0)
			title_choices = title_loop();
		else if (game_result == 1 || game_result == 2)  // リトライ
			title_choices = 0;

		switch (title_choices)
		{
			case 0:
				game_result = game_loop();
				break;

			case 1:
				option_loop();
				break;

			case 2:
				is_continue = false;
				break;

			default:
				break;
		}

		if (game_result == 1)  // loop()==0: リトライ
			game_result = (gameover_loop() == 0 ? game_result : 0);
		else if (game_result == 2)
			game_result = (gameclear_loop() == 0 ? game_result : 0);
	}
	return 0;
}