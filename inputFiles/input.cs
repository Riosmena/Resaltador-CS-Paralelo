using System;

class Program {
    static void Main(string[] args) {
        // Pide al usuario que ingrese un número arábigo
        Console.WriteLine("Ingrese un número arábigo:");
        int numArabigo = int.Parse(Console.ReadLine());
        
        // Convierte el número arábigo a número romano
        string numRomano = ConvertArabigoToRoman(numArabigo);
        
        // Muestra el número romano resultante
        Console.WriteLine("El número romano es: " + numRomano);
    }
    
    // Función para convertir números arábigos a números romanos
    static string ConvertArabigoToRoman(int number) {
        if (number < 1 || number > 3999) {
            throw new ArgumentException("Número fuera de rango.");
        }
        
        string[] thousands = {"", "M", "MM", "MMM"};
        string[] hundreds = {"", "C", "CC", "CCC", "CD", "D", "DC", "DCC", "DCCC", "CM"};
        string[] tens = {"", "X", "XX", "XXX", "XL", "L", "LX", "LXX", "LXXX", "XC"};
        string[] ones = {"", "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX"};
        
        string result = "";
        
        // Agrega los miles
        int thousandsDigit = number / 1000;
        result += thousands[thousandsDigit];
        number %= 1000;
        
        // Agrega las centenas
        int hundredsDigit = number / 100;
        result += hundreds[hundredsDigit];
        number %= 100;
        
        // Agrega las decenas
        int tensDigit = number / 10;
        result += tens[tensDigit];
        number %= 10;
        
        // Agrega las unidades
        int onesDigit = number;
        result += ones[onesDigit];
        
        return result;
    }
}