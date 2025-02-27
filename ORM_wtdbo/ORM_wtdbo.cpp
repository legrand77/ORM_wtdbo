
#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/backend/Postgres.h>
#include <iostream>
#include <Windows.h>

#pragma execution_character_set("utf-8");

using namespace std;

class Stock;
class Publisher;
class Sale;
class Shop;

class Book {
public:
	string title = "";
	Wt::Dbo::collection< Wt::Dbo::ptr<Stock> > stock;
	Wt::Dbo::ptr<Publisher> publishers;

	template<class Action>
	void persist(Action& a)
	{
		Wt::Dbo::field(a, title, "title");
		Wt::Dbo::belongsTo(a, publishers, "publisher");
		Wt::Dbo::hasMany(a, stock, Wt::Dbo::ManyToOne, "book");
	}
};
class Stock {
public:
	string count = "";
	Wt::Dbo::ptr<Book> book;
	Wt::Dbo::ptr<Shop> shop;
	Wt::Dbo::collection< Wt::Dbo::ptr<Sale> > sale;

	template<class Action>
	void persist(Action& a)
	{
		Wt::Dbo::field(a, count, "count");
		Wt::Dbo::belongsTo(a, book, "book");
		Wt::Dbo::belongsTo(a, shop, "shop");
		Wt::Dbo::hasMany(a, sale, Wt::Dbo::ManyToOne, "stock");
	}
};
class Sale {
public:

	string price = "";
	string date_sale = "";
	string count = "";
	Wt::Dbo::ptr<Stock> stock;

	template<class Action>
	void persist(Action& a)
	{
		Wt::Dbo::field(a, price, "price");
		Wt::Dbo::field(a, date_sale, "date_sale");
		Wt::Dbo::field(a, count, "count");
		Wt::Dbo::belongsTo(a, stock, "stock");
	}
};
class Publisher {
public:
	string name = "";
    Wt::Dbo::collection<Wt::Dbo::ptr<Book>> books;

	template<class Action>
	void persist(Action& a)
	{
		Wt::Dbo::field(a, name, "name");
		Wt::Dbo::hasMany(a, books, Wt::Dbo::ManyToOne, "publisher");
	}
};
class Shop {
public:
	string name = ""; 
	Wt::Dbo::collection< Wt::Dbo::ptr<Stock> > stock;

	template<class Action>
	void persist(Action& a)
	{
		Wt::Dbo::field(a, name, "name");
		Wt::Dbo::hasMany(a, stock, Wt::Dbo::ManyToOne, "shop");
	}
};
int main() {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
	try {
		string connectionString =
			"host=localhost "
			"port=5432 "
			"dbname=customerManagement "
			"user=postgres "
			"password=123";

		auto postgres = make_unique<Wt::Dbo::backend::Postgres>(connectionString);
		Wt::Dbo::Session session;

		session.setConnection(std::move(postgres));
		session.mapClass<Publisher>("publisher");
		session.mapClass<Book>("book");
		session.mapClass<Stock>("stock");
		session.mapClass<Shop>("shop");
		session.mapClass<Sale>("sale");

		vector<string> publisherName{"Pushkin", "Lermontov", "Dostoevsky", "Nekrasov", "Krylov"};              
		vector<string> ShopName{ "Shop_1", "Shop_2", "Shop_3", "Shop_4", "Shop_5" };                   
		vector<string> Saleprice{ "price_1", "price_2",  "price_3", "price_4", "price_5" };                       
		vector<string> Saledate_sale{ "date_sale_1", "date_sale_2", "date_sale_3", "date_sale_4", "date_sale_5" };                     
		vector<string> Salecount{ "salecount_1", "salecount_2", "salecount_3", "salecount_4", "salecount_5" };					  
		vector<string> Stockcount{ "stockcount_1", "stockcount_2", "stockcount_3", "stockcount_4", "stockcount_5" };					   
		vector<string> Booktitle{ "book_1", "book_2", "book_3", "book_4", "book_5" };					   
		vector<string> publisherName_1{ "Pushkin", "Pushkin", "Lermontov", "Lermontov", "Dostoevsky" };
		
		int op = -1;
	   
		    while (op != 0) {
				try {
				  Wt::Dbo::Transaction transaction{session};
			      cout << "Введите операцию:\n" <<
				  "1.Создание и заполнение таблиц\n" <<
				  "2.Запрос выборки магазинов, продающих целевого издателя.\n" <<
				  "0.Выход\n";
				   cin >> op;
				   switch (op) {
				     case 1: {
					   cout << "Создание и заполнение таблиц" << endl;
					   session.createTables();
					   
					   for (int i = 0; i < 5; i++) {

						   unique_ptr<Publisher> publisher{new Publisher()};
					       publisher->name = publisherName[i];
					       Wt::Dbo::ptr<Publisher> publisherPtr = session.add(move(publisher));
						   
						   unique_ptr<Shop> shop{ new Shop() };
						   shop->name = ShopName[i];
						   Wt::Dbo::ptr<Shop> shopPtr = session.add(move(shop));

                           Wt::Dbo::ptr<Book> book = session.add(std::unique_ptr<Book>{new Book()});
						   Wt::Dbo::ptr<Publisher> publisher1 = session.find<Publisher>().where("name = ?").bind(publisherName_1[i]);
                           book.modify()->title = Booktitle[i];
						   book.modify()->publishers = publisher1;

						   Wt::Dbo::ptr<Stock> stock = session.add(std::unique_ptr<Stock>{new Stock()});
						   Wt::Dbo::ptr<Shop> shop1 = session.find<Shop>().where("name = ?").bind(ShopName[i]);
                           Wt::Dbo::ptr<Book> book1 = session.find<Book>().where("title = ?").bind(Booktitle[i]);
						   stock.modify()->shop = shop1;
						   stock.modify()->book = book1;
						   stock.modify()->count = Stockcount[i];
						   
						   Wt::Dbo::ptr<Sale> sale = session.add(std::unique_ptr<Sale>{new Sale()});
						   Wt::Dbo::ptr<Stock> stock1 = session.find<Stock>().where("id = ?").bind(i+1);
						   sale.modify()->price = Saleprice[i];
						   sale.modify()->date_sale = Saledate_sale[i];
						   sale.modify()->count = Salecount[i];
						   sale.modify()->stock = stock1;
					   }
                       transaction.commit();
					   break;
				     }
					 case 2: {
						 string x{};
						 int y{};
						 int count{0};
						 cout << "Введите позицию издателя.\n" << endl;
						 for (auto& i : publisherName) {
							 cout << ++count << "-" << i << endl;
						 } 
						 cin >> x;
						 y = stoi(x);
						 if (y<1 || y>5 ) break;
						 Wt::Dbo::collection<Wt::Dbo::ptr<Stock>> stock = session.find<Stock>().orderBy("id");
						 for (const Wt::Dbo::ptr<Stock>& stock : stock)
						 {
							 if (stock->book->publishers->name == publisherName[y - 1])
							 cout << stock->shop->name << endl;
						 }
						 transaction.commit();
					    break;
					 }
					 default:
					    cout << "Команды не существует" << endl;

				   }
				}catch (const exception& e) {
				    cout << "Ошибки запроса "<< e.what() << endl;
		        }

			}
	}
	catch (const std::exception& e) {
		cout << "Ошибки подключения " << e.what() << endl;

	}

 return EXIT_SUCCESS;
}
