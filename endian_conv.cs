/* endian_conv.cs -- convert a group of files, or any file in a directory with
 * one of several extensions, to a certain encoding.
 *
 * To the extent possible under law, the author has dedicated all copyright and related and neighboring rights to endian_conv to the public domain worldwide. This software is distributed without any warranty.
 *
 * The CC0 Public Domain Dedication applies to this file - see <http://creativecommons.org/publicdomain/zero/1.0/>. */

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace endian_conv {
	class endian_conv {
		static void usage() {
			Console.Error.WriteLine(@"Usage: endian_conv {encoding} [files ...]
   or: endian_conv {encoding} [directory] [extensions ...]

{encoding} is the output encoding - uses the Encoding.GetEncoding() method.
Use utf-8 for UTF-8, utf-16 for UTF-16 (little endian).
utf-8 is a special case - this program will NOT write a BOM for UTF-8.");
			Environment.Exit(1);
		}

		static void Main(string[] args) {
			if (args.Length < 2) {
				usage();
			}
			Encoding target;
			if (args[0] == "utf-8") {
				target = new UTF8Encoding(false);
			} else {
				target = Encoding.GetEncoding(args[0]);
			}
			List<string> toProcess = new List<string>();
			if (Directory.Exists(args[1])) {
				for (int i = 2; i < args.Length; i++) {
					toProcess.AddRange(Directory.EnumerateFiles(args[1], args[i]));
				}
			} else {
				for (int i = 1; i < args.Length; i++) {
					if (!File.Exists(args[i])) {
						Console.Error.WriteLine(args[i] + " is not a file or does not exist");
						usage();
					}
					toProcess.Add(args[i]);
				}
			}
			Console.Write("Converting to " + target.WebName + ": ");
			foreach (string filename in toProcess) {
				string native = File.ReadAllText(filename);
				var time = File.GetLastWriteTime(filename);
				Console.Write(Path.GetFileName(filename) + " ");
				File.WriteAllText(filename, native, target);
				File.SetLastWriteTime(filename, time);
			}
			Console.WriteLine();
		}
	}
}
