完成了 UW programming languages(a) 的 assignment - 1

```c
fun is_older ((y1, m1, d1) : (int * int * int), (y2, m2, d2) : (int * int * int)) =
		if y1 < y2 then true
		else if y1 = y2 then
				if m1 < m2 then true
				else if m1 = m2 then
						d1 < d2
					else false
			else false

fun number_in_month ([], _) = 0
	| number_in_month ((y, m, d)::dates, month) =
		if m = month 
		then 1 + number_in_month (dates, month)
		else number_in_month (dates, month)

fun number_in_months (_, []) = 0
	| number_in_months (dates, month::months) =
		number_in_month(dates, month) + number_in_months(dates, months)

fun dates_in_month ([], _) = []
	| dates_in_month ((y, m, d)::dates, month) =
		if m = month
		then (y, m, d)::dates_in_month(dates, month)
		else dates_in_month(dates, month)

fun dates_in_months (_, []) = []
	| dates_in_months (dates, month::months) =
		dates_in_month(dates, month)::dates_in_months(dates, months)

fun get_nth (x::xs, 1) = x
	| get_nth (x::xs, n) = get_nth(xs, n-1)

val months = ["January", "February", "March", "April", "May", "June", 
		"July", "August", "September", "October", "November", "December"]

fun date_to_string ((year, month, day): (int * int * int)) =
		(get_nth(months, month)) ^ " " ^ Int.toString(day) ^ ", " ^ Int.toString(year)

fun number_before_reaching_sum (_, []) = 0
	| number_before_reaching_sum (n, x::xs) =
		let
			fun helper (s, count, []) = count
				| helper (s, count, y::ys) = 
					if s + y < n then helper(s + y, count + 1, ys)
					else count
		in
			helper (0, 0, x::xs)
		end

val month_days = [31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31]
fun what_month (n) = 
		let
			fun helper (count, n) =
					if n > get_nth(month_days, count)
					then helper (count + 1, n - get_nth(month_days, count))
					else count
		in
			helper(1, n)
		end
	
fun month_range (d1, d2) =
		if d1 > d2 then []
		else what_month(d1) :: month_range (d1 + 1, d2)

fun oldest dates = 
		let
			fun helper (elder, []) = elder
				| helper (NONE, x::xs) = helper (SOME x, xs)
				| helper (SOME elder, x::xs) =
					if is_older(elder, x) then helper (SOME elder, xs)
					else helper (SOME x, xs)
		in
			helper (NONE, dates)
		end

fun remove_duplicates [] = []
	| remove_duplicates (x::xs) = 
		let
			fun exists (which, []) = false
				| exists (which, x::xs) = 
					if which = x then true
					else exists (which, xs)

			fun helper (res, []) = res
				| helper (res, x::xs) = 
					if exists (x, res)
					then helper(res, xs)
					else helper(x::res, xs)
		in
			helper([], x::xs)
		end

fun number_in_months_challenge (dates, months) = 
		number_in_months (dates, remove_duplicates(months))

fun dates_in_months_challenge (dates, months) = 
		dates_in_months (dates, remove_duplicates(months))

fun is_leap_year year =
		(year mod 4 = 0) andalso ((year mod 100 <> 0) orelse (year mod 400 = 0))

fun days_in_month (month, year) =
		case month of
			2 => if is_leap_year year then 29 else 28
		| 4 => 30 | 6 => 30 | 9 => 30 | 11 => 30
		| _ => 31

fun reasonable_date (year, month, day) =
		year > 0 andalso month >= 1 andalso month <= 12 andalso day >= 1 andalso day <= days_in_month(month, year)
```