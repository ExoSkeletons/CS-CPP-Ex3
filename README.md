# Coup
מרד

## Instructions
Follow instructons as they show up

To compile - Run ```make main``` | ```make valgrind``` as required

## תכנון מערכת המשחק

המערכת תוכננה עם שימוש רב במחלקות, וירטואליזציה, ירושה ועוד כל מיני דברים מעניינים.
המערכת בנויה ותוכננה בצורה שניתן בקלות להרחיב את המשחק, להוסיף שחקנים נוספים ופעולות שונות בקלות רבה.

* Player
  מחלקה שמייצגת שחקן. למחלקה מספר שדות לעקיבה אחרי מצב השחקן (blocked, sanctioned, arrested, etc.) ומספר מתודות המופעלות במהלך המשחק בהתאם למהלך התורים (onTurnStart/End, etc.)
* Action
  מחלקה המייצגת פעולה שניתן לבצע. שדות עיקריים  - Player actor, target
* Game
  שליטה במהלך המשחק. פונקציות עיקריות - isWin, nextTurn, playTurn, add/removePlayer

## בעניין ה-GUI

לא הספקתי לממש GUI. אף אחד אחר גם לא הספיק לממש GUI. מי שכן יש לו GUI, פשוט העתיק אותו מ-GPT. זה לא סוד.

באופן אישי, אני הייתי חולה כל השבוע האחרון - כך שלא היה לי את הזמן והכוח ללמוד ספריית GUI ולא את הכוח להעתיק מ-GPT, וכתבתי UI פשוט.

בכל מקרה, השתדלתי להפריד את המתודות ולתכנן את המערכת כך שניתן יהיה "יחסית בקלות" לתפעל אותה מאיזשהו GUI היפותטי. בכל מקרה למוטב לציין שמאחר והייתי חולה מדי להעתיק מ-GPT, הקוד שלפניכם, והמערכת המתוכנתת כאן מובאת לפניכם כתוצר של 100% דם יזע ודמעות שלי ואשמח לספר לכם את סיפור חייו.

מצד הבדיקה, מאחר ועיקר בדיקת המטלה (לפחות לפי מה שאלינה אמרה) היא לבדוק את יכולות עיצוב ה-backend, ומאחר והייתי חולה כל השבוע, אשמח להתחשבות כמה שאפשר :)

בברכה
