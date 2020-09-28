#include "Prog1.hpp"

namespace Prog1
{
	Matrix input(std::istream& stream)
	{
		const char* pr = ""; // будущее сообщение об ошибке
		int m;							 // количество строк матрицы
		int n;
		// сначала вводим количество строк
		do
		{
			std::cout << pr << std::endl;
			std::cout << "Enter number of lines: --> ";
			pr = "You are wrong; repeat please!";
			if (getNum(m, stream) < 0) // обнаружена ошибка ввода или конец файла
				return Matrix(0, 0);
		} while (m < 1);
		pr = "";
		do
		{
			std::cout << pr << std::endl;
			std::cout << "Enter number of columns: --> ";
			pr = "You are wrong; repeat please!";
			if (getNum(n, stream) < 0) // обнаружена ошибка ввода или конец файла
				return Matrix(0, 0);
		} while (n < 1);
		pr = "";
		Matrix mat(m, n);

		for (int i = 0; i < m; i++)
		{
			// теперь можно вводить сами элементы данной строки матрицы
			std::cout << "Enter items for matrix line #" << (i + 1) << ":" << std::endl;
			mat.lines.push_back({});
			auto& line = mat.lines.back();
			for (int j = 0; j < n; j++)
			{
				double elem;
				if (getNum(elem, stream) < 0)
				{ // обнаружена ошибка ввода или конец файла
					return Matrix(0, 0);
				}
				if (elem != 0)
					line.push_back({ j, elem });
			}
		}
		return mat;
	}

	void Matrix::output(std::string const& msg)
	{
		std::cout << msg << std::endl;
		for (int i = 0; i < m; i++)
		{
			auto elem_it = lines[i].begin(); // итератор начала вектора
			for (int j = 0; j < n; j++)
			{
				if (elem_it != lines[i].end() && elem_it->x == j) // проверяем, что не конец ненулевой строки и номер текущего элемента совпадает с j
				{
					std::cout << elem_it->val << "  ";
					elem_it++; // переходим к след элементу вектора
				}
				else
					std::cout << "0  ";
			}
			std::cout << "\n";
		}
		std::cout.flush(); // сброс буфера
	}
	// функция нахождения первого(последнего) максимального(минимального), выбор: sec
	void findnum(std::vector<MatItem>& line, int const& n, bool sec, int& a, int& a_place)
	{
		// сохраняем пару чисел, первое из которых означает позицию в векторе(или что его нет в векторе), а второе на позицию в строке реальной матрицы
		// -2 * - нет i или j
		// -1 p - найден нужный элемент и равен 0
		// k * - k>=0 найден элемент в векторе
		a = -2;
		a_place = 0;
		int prev_val = line[0].x == 0 ? line[0].val : 0; // либо 0, либо первый элемент из вектора(если он стоит на 0 месте в реальной матрице)
		for (int x = 0, x_ = 0; x < n; x++)
		{
			if (x_ < line.size() && x == line[x_].x)
			{
				if (sec ? line[x_].val < prev_val : line[x_].val > prev_val) // выбор больше или меньше предыдущего в зависимости от sec
				{
					a = x_;
					a_place = 0; // номер в реальной матрице не важен, сохраняем только номер в векторе
					if (!sec)			 // возвращаем значение сразу, если нужно вернуть первый, и идем дальше, если последний
						return;
				}

				prev_val = line[x_].val; // переписываем предыдущее значение
				x_++;										 // шаг вперед по вектору
			}
			else
			{
				if (sec ? 0 < prev_val : 0 > prev_val)
				{
					a = -1;
					a_place = x; // говорим, что числа нет в векторе(тк оно == 0), записываем номер в реальной матрице
					if (!sec)
						return;
				}
				prev_val = 0;
			}
		}
	}

	void swapij(std::vector<MatItem>& line, int const& n)
	{
		int i, i_place, j, j_place;
		if (line.size() == 0)
			return;
		else
		{
			findnum(line, n, 0, i, i_place); // вызываем функцию для поиска первого элемента, который больше предыдущего, и записываем в i, i_place
			findnum(line, n, 1, j, j_place); // тоже самое для последнего меньшего

			if (i == -2 || j == -2 || (i == -1 && j == -1)) // если оба нули или вообще какой-то не нашли, выходим из функции
				return;
			if (i != -1 && j != -1) // если нашлись оба, меняем местами значения
				std::swap(line[i].x, line[j].x);
			if (j != -1 && i == -1) // нашелся только j, а i нулевой(на него забиваем просто)
				line[j].x = i_place;
			if (i != -1 && j == -1) // аналогично
				line[i].x = j_place;
			std::sort(line.begin(), line.end()); // сортируем по MatItem::x (23 строка Prog1.hpp позволяет выполнить сравнение элементов)
		}
	}

	Matrix swapij(Matrix& mat)
	{
		Matrix ret{ mat.m, mat.n }; // новая матрица
		for (int i = 0; i < mat.m; i++)
		{
			// заполняем
			auto elem_it = mat.lines[i].begin();
			ret.lines.push_back({});
			auto& line = ret.lines.back();
			while (elem_it != mat.lines[i].end())
			{
				line.push_back({ elem_it->x, elem_it->val });
				elem_it++;
			}
			// вызываем функцию смены местами
			swapij(line, mat.n);
		}
		return ret;
	}
	// оператор сравнения матриц, для отладки и тестирования
	/*bool Matrix::operator==(Matrix const& mat)
	{
		bool err = 0;
		if (mat.m != m || mat.n != n)
		{
			std::cerr << "size mismatch";
			err = 1;
		}
		for (int i = 0; i < m; i++)
		{
			if (lines[i].size() != mat.lines[i].size())
			{
				std::cerr << "size mismatch @ line #" << i << " (" << lines[i].size() << "!=" << mat.lines[i].size() << ")" << std::endl;
				err = 1;
			}
			for (int j = 0; j < lines[i].size(); j++)
			{
				if (mat.lines[i][j] != lines[i][j])
				{
					std::cerr << "elements not matching @ line #" << i << " pos #" << j << "(" << lines[i][j] << "!=" << mat.lines[i][j] << ")" << std::endl;
					err = 1;
				}
			}
		}
		return !err;
	}*/
} // namespace Prog1
