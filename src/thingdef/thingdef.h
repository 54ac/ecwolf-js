/*
** thingdef.h
**
**---------------------------------------------------------------------------
** Copyright 2011 Braden Obrzut
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**---------------------------------------------------------------------------
**
**
*/

#ifndef __THINGDEF_H__
#define __THINGDEF_H__

#include "actor.h"
#include "tarray.h"
#include "wl_def.h"
#include "zstring.h"

class ClassDef;
class Frame;
class Symbol;
class ExpressionNode;
class Type;
class Scanner;

class StateLabel
{
	public:
		StateLabel() : isRelative(false) {}
		StateLabel(const FString &str, const ClassDef *parent, bool noRelative=false);
		StateLabel(Scanner &sc, const ClassDef *parent, bool noRelative=false);

		const Frame	*Resolve() const;
		const Frame	*Resolve(AActor *self) const;

	private:
		void	Parse(Scanner &sc, const ClassDef *parent, bool noRelative=false);

		const ClassDef	*cls;
		FString 		label;
		unsigned short	offset;
		bool			isRelative;
};

class CallArguments
{
	public:
		class Value
		{
			public:
				enum
				{
					VAL_INTEGER,
					VAL_DOUBLE,
					VAL_STRING,
					VAL_STATE
				} useType;
				bool isExpression;

				ExpressionNode	*expr;
				union
				{
					int64_t	i;
					double	d;
				}				val;
				FString			str;
				StateLabel		label;
		};

		~CallArguments();

		void		AddArgument(const Value &val);
		int			Count() const { return args.Size(); }
		void		Evaluate(AActor *self);
		const Value	&operator[] (unsigned int idx) const { return args[idx]; }
	private:
		TArray<Value> args;
};

class ActionInfo
{
	public:
		ActionInfo(ActionPtr func, const FName &name);

		ActionPtr func;
		const FName name;

		unsigned int					minArgs;
		unsigned int					maxArgs;
		bool							varArgs;
		TArray<CallArguments::Value>	defaults;
		TArray<const Type *>			types;
};

typedef TArray<ActionInfo *> ActionTable;

#define ACTION_FUNCTION(func) \
	void __AF_##func(AActor *self, const CallArguments &args); \
	static const ActionInfo __AI_##func(__AF_##func, #func); \
	void __AF_##func(AActor *self, const CallArguments &args)
#define ACTION_ALIAS(func, alias) \
	ACTION_FUNCTION(alias) \
	{ \
		__AF_##func(self, args); \
	}
#define ACTION_PARAM_COUNT args.Count()
#define ACTION_PARAM_BOOL(name, num) \
	bool name = args[num].val.i ? true : false
#define ACTION_PARAM_INT(name, num) \
	int name = args[num].val.i
#define ACTION_PARAM_DOUBLE(name, num) \
	double name = args[num].val.d
#define ACTION_PARAM_STRING(name, num) \
	FString name = args[num].str
#define ACTION_PARAM_STATE(name, num) \
	const Frame *name = args[num].label.Resolve(self)

class SymbolInfo
{
	public:
		SymbolInfo(const ClassDef *cls, const FName &var, const int offset);

		const ClassDef	* const cls;
		const FName		var;
		const int		offset;
};

#define DEFINE_SYMBOL(cls, var) \
	static const SymbolInfo __SI_##var(NATIVE_CLASS(cls), #var, typeoffsetof(A##cls,var));

struct StateDefinition;

struct PropertyParam
{
	bool isExpression;
	union
	{
		ExpressionNode	*expr;
		char			*s;
		double			f;
		int64_t			i;
	};
};
typedef void (*PropHandler)(ClassDef *info, AActor *defaults, const unsigned int PARAM_COUNT, PropertyParam* params);
#define HANDLE_PROPERTY(property) void __Handler_##property(ClassDef *cls, AActor *defaults, const unsigned int PARAM_COUNT, PropertyParam* params)
struct PropDef
{
	public:
		const ClassDef* const	&className;
		const char* const		prefix;
		const char* const		name;
		const char* const		params;
		PropHandler				handler;
};

typedef TArray<Symbol *> SymbolTable;

class MetaTable
{
	public:
		MetaTable();
		MetaTable(const MetaTable &other);
		~MetaTable();

		enum Type
		{
			INTEGER,
			FIXED,
			STRING
		};

		int			GetMetaInt(uint32_t id, int def=0) const;
		fixed		GetMetaFixed(uint32_t id, fixed def=0) const;
		const char*	GetMetaString(uint32_t id) const;
		void		SetMetaInt(uint32_t id, int value);
		void		SetMetaFixed(uint32_t id, fixed value);
		void		SetMetaString(uint32_t id, const char* value);

	private:
		class Data;

		Data	*head;
		Data	*FindMeta(uint32_t id) const;
		Data	*FindMetaData(uint32_t id);

		void	FreeTable();
};

class ClassDef
{
	public:
		ClassDef();
		~ClassDef();

		AActor					*CreateInstance() const;
		bool					IsDescendantOf(const ClassDef *parent) const;

		/**
		 * Use with IMPLEMENT_CLASS to add a natively defined class.
		 */
		template<class T>
		static const ClassDef	*DeclareNativeClass(const char* className, const ClassDef *parent)
		{
			ClassDef **definitionLookup = ClassTable().CheckKey(className);
			ClassDef *definition = NULL;
			if(definitionLookup == NULL)
			{
				definition = new ClassDef();
				ClassTable()[className] = definition;
			}
			else
				definition = *definitionLookup;
			definition->name = className;
			definition->parent = parent;
			definition->defaultInstance->defaults = NULL;
			definition->defaultInstance->~AActor();
			free(definition->defaultInstance);
			definition->defaultInstance = (AActor *) malloc(sizeof(T));
			definition->defaultInstance->defaults = definition->defaultInstance;
			definition->ConstructNative = &T::__InPlaceConstructor;
			return definition;
		}

		typedef TMap<FName, ClassDef*>::ConstIterator	ClassIterator;
		typedef TMap<FName, ClassDef*>::ConstPair		ClassPair;
		static ClassIterator	GetClassIterator()
		{
			return ClassIterator(ClassTable());
		}

		/**
		 * Prints the implemented classes in a tree.  This is not designed to 
		 * be fast since it's debug information more than anything.
		 */
		static void				DumpClasses();

		static const ClassDef	*FindClass(unsigned int ednum);
		static const ClassDef	*FindClass(const FName &className);
		static const ClassDef	*FindClassTentative(const FName &className, const ClassDef *parent);
		const ActionInfo		*FindFunction(const FName &function) const;
		const Frame				*FindState(const FName &stateName) const;
		Symbol					*FindSymbol(const FName &symbol) const;
		AActor					*GetDefault() const { return defaultInstance; }
		const FName				&GetName() const { return name; }
		static void				LoadActors();
		static void				UnloadActors();

		MetaTable				Meta;

	protected:
		friend class StateLabel;

		static void	ParseActor(Scanner &sc);
		static void	ParseDecorateLump(int lumpNum);
		static bool	SetFlag(ClassDef *newClass, const FString &prefix, const FString &flagName, bool set);
		static bool SetProperty(ClassDef *newClass, const char* className, const char* propName, Scanner &sc);

		const Frame * const *FindStateInList(const FName &stateName) const;
		void		InstallStates(const TArray<StateDefinition> &stateDefs);

		// We need to do this for proper initialization order.
		static TMap<FName, ClassDef *>	&ClassTable();
		static TMap<int, ClassDef *>	classNumTable;
		static SymbolTable				globalSymbols;

		bool			tentative;
		FName			name;
		const ClassDef	*parent;

		TMap<FName, unsigned int>	stateList;
		TArray<Frame *>			frameList;

		ActionTable		actions;
		SymbolTable		symbols;

		AActor			*defaultInstance;
		AActor			*(*ConstructNative)(const ClassDef *, void *);
};

#endif /* __THINGDEF_H__ */