/* テスト用: -DBONUS の有無でヘッダを切り替える */
#ifdef BONUS
# include "get_next_line_bonus.h"
#else
# include "get_next_line.h"
#endif
